#pragma once
#include <Component/SkinnedMeshRenderer.h>
#include <ResourceManager.h>
#include <Component/Transform.h>
#include <GameObject.h>
#include <RenderingSystem.h>
////////////////////////////////////////////////////////////////////////////////

SkinnedMeshRenderer::SkinnedMeshRenderer(const char* fileName)
{
	m_VAO = 0;
	memset(m_Buffers, 0, sizeof(m_Buffers[0])* NUM_VBs);
	m_NumBones = 0;
	m_NumClothBones = 128;
	m_pScene = NULL;

	
	m_crtRootTransform = m_prevRootTransform = glm::mat4(1);
	
	m_crtLeftHandTransform = m_prevLeftHandTransform = glm::mat4(1);
	m_crtRightHandTransform = m_prevRightHandTransform = glm::mat4(1);
	
	m_crtLeftAnkleTransform = m_prevLeftHandTransform = glm::mat4(1);
	m_crtRightAnkleTransform = m_prevRightHandTransform = glm::mat4(1);
	
	m_crtLeftForearmTransform = m_prevLeftForearmTransform = glm::mat4(1);
	m_crtRightForearmTransform = m_prevRightForearmTransform = glm::mat4(1);

	m_crtLeftShinTransform = m_prevLeftShinTransform = glm::mat4(1);
	m_crtRightShinTransform = m_prevRightShinTransform = glm::mat4(1);


	LoadMesh(fileName);

	{
		{
			RenderSystem &renderSystem = RenderSystem::getInstance();
			std::vector<glm::vec3> points = { glm::vec3(-1.5, -1, -1), glm::vec3(1, -1, -1),glm::vec3(1, 3, -1), glm::vec3(-1, 3, -1), 
											  glm::vec3(-1, -1,  1), glm::vec3(1, -1,  1),glm::vec3(1, 3,  1), glm::vec3(-1, 3,  1) };
			btCollisionShape* groundShape = new btConvexHullShape((const btScalar *)(&(points[0][0])), points.size(), 3 * sizeof(btScalar));

			renderSystem.m_collisionShapes.push_back(groundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, 0, 0));

			btScalar mass(0.);
			btVector3 localInertia(0, 0, 0);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);

			//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);

			renderSystem.m_collisionWorld->addRigidBody(body);// , group, mask);
			mFrustumCollisionObject = (btCollisionObject*)body;
		}
	}


	m_animaBoneStruct = new SkelTreeAnimTrack();
	RegisterSkeletalAnim(m_pScene->mRootNode, m_animaBoneStruct);
	m_rootPose = new SkelTreePose;
	m_tempRootPose = new SkelTreePose;
	InitPose(m_animaBoneStruct, m_rootPose);
	InitPose(m_animaBoneStruct, m_tempRootPose);

	RenderSystem &renderSys = RenderSystem::getInstance();
	renderSys.mSkinnedMeshes.push_back(this);
	
	delete m_animaBoneStruct;
}

////////////////////////////////////////////////////////////////////////////////

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	delete m_animaBoneStruct;
	Clear();
}

////////////////////////////////////////////////////////////////////////////////

DualQuaternion
SkinnedMeshRenderer::QuatTrans2UDQ(const glm::quat &q0, const glm::vec3 &t)
{
	DualQuaternion dq;
	// non-dual part (just copy q0):
	dq[0][0] = q0.w;
	dq[0][1] = q0.x;
	dq[0][2] = q0.y;
	dq[0][3] = q0.z;
	// dual part:
	dq[1][0] = -0.5f * (t.x * q0.x + t.y * q0.y + t.z * q0.z);
	dq[1][1] = 0.5f * (t.x * q0.w + t.y * q0.z - t.z * q0.y);
	dq[1][2] = 0.5f * (-t.x * q0.z + t.y * q0.w + t.z * q0.x);
	dq[1][3] = 0.5f * (t.x * q0.y - t.y * q0.x + t.z * q0.w);
	return dq;
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::sendUniforms(Shader *shader) {
	glUniformMatrix2x4fv(shader->GetUniformLocation("boneDQ"), m_NumBones, false, &dualQuaternions[0][0][0]);
	//assert(currentTransforms.size() == m_NumBones);
	//glUniformMatrix4fv(shader->GetUniformLocation["skinning_matrices"], currentTransforms.size(), false, &(currentTransforms[0][0][0]));
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::prepareUniforms()
{
	for (uint j = 0; j < m_NumBones; j++)
	{
		glm::quat q = glm::toQuat(glm::mat3(currentTransforms[j]));
		glm::vec3 t = glm::vec3(currentTransforms[j][3][0], currentTransforms[j][3][1], currentTransforms[j][3][2]);
		DualQuaternion dq = QuatTrans2UDQ(q, t);
		dualQuaternions[j] = dq;
	}
}

////////////////////////////////////////////////////////////////////////////////

bool SkinnedMeshRenderer::LoadMesh(const char* fileName)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(NUM_VBs, m_Buffers);

	bool Ret = false;
	ResourceManager &resMgr = ResourceManager::getInstance();
	m_pScene = resMgr.mImporter.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (m_pScene)
	{
		m_GlobalInverseTransform = convertMatrix4x4(m_pScene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		Ret = InitFromScene(m_pScene, fileName);
	}
	else {
		printf("Error parsing '%s': '%s'\n", fileName, resMgr.mImporter.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
	return Ret;
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::Skin(Shader *m_TFshader)
{
	glBindVertexArray(m_VAO);
	
	const GLint POSITION_LOCATION = m_TFshader->GetAttributeLocation("in_position");
	const GLint TEX_COORD_LOCATION = m_TFshader->GetAttributeLocation("TexCoord");
	const GLint NORMAL_LOCATION = m_TFshader->GetAttributeLocation("in_normal");
	const GLint TANGENT_LOCATION = m_TFshader->GetAttributeLocation("in_tangent");
	const GLint BONE_ID_LOCATION = m_TFshader->GetAttributeLocation("BoneIDs");
	const GLint BONE_WEIGHT_LOCATION = m_TFshader->GetAttributeLocation("Weights");

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
	glEnableVertexAttribArray(TANGENT_LOCATION);
	glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribPointer(BONE_ID_LOCATION, 4, GL_INT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(4 * sizeof(GLint)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);//??????

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TFVBO);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TFVBO);
	//glBeginTransformFeedback(GL_TRIANGLES);
	glBeginTransformFeedback(GL_POINTS);
	/*for (uint i = 0; i < m_Entries.size(); i++)
	{
		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}*/

	glDrawArrays(GL_POINTS, 0, NumVertices);
	glEndTransformFeedback();
	glFlush();
	glBindVertexArray(0);
	if (mCPUskinning)
		SkinOnCPU();
}

#include <time.h>
void SkinnedMeshRenderer::SkinOnCPU()
{//TODO
	//clock_t tStart = clock();
	for (uint i = 0; i < mClothPositions.size(); i++)
	{		
		const glm::mat4 &BoneTransform = currentTransforms[mClothBones[i].IDs[0]];
		

		//mat4 scalemat = mat4(0.0328084, 0, 0, 0, 0, 0.0328084, 0, 0, 0, 0, 0.0328084, 0, 0, 0, 0, 1);
		mSkinnedClothPositions[i] = glm::vec3(BoneTransform * glm::vec4(mClothPositions[i], 1));
		mSkinnedClothNormals[i] = glm::vec3(BoneTransform * glm::vec4(mClothNormals[i], 0));
		mSkinnedClothTangents[i] = glm::vec3(BoneTransform * glm::vec4(mClothTangents[i], 0));
	}
	//printf("CPU SKIN Time: %lf seconds\n\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	glBindVertexArray(m_clothVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mSkinnedClothPositions[0]) * 
				 mSkinnedClothPositions.size(), &mSkinnedClothPositions[0][0], GL_DYNAMIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mSkinnedClothNormals[0]) *
				 mSkinnedClothNormals.size(), &mSkinnedClothNormals[0][0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mSkinnedClothTangents[0]) *
		mSkinnedClothTangents.size(), &mSkinnedClothTangents[0][0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TEXCOORD_VB]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ClothBuffers[CLOTH_INDEX_BUFFER]);//??????

	glBindVertexArray(0);

}
void SkinnedMeshRenderer::UseMaterial(Shader *shader, int matID)
{
	Material &mat = mMaterials[matID];
	if (mat.diffuseMap >= 0)
	{
		glUniform1i(shader->GetUniformLocation(("uv_maps.diffuseMapON")), 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, mat.diffuseMap);
		glUniform1i(shader->GetUniformLocation(("diffuseMap")), 1);
	}
	else
	{
		glUniform1i(shader->GetUniformLocation(("uv_maps.diffuseMapON")), 0);
		//glUniform1i(shaders["gBufferPassShader"]->GetUniformLocation(("uv_maps.bumpMapON")), tex.bumpMapON);
		//glUniform1i(shaders["gBufferPassShader"]->GetUniformLocation(("uv_maps.alphaMapON")), tex.alphaMapON);
		//glUniform1i(shader->GetUniformLocation(("uv_maps.specularMapON")), tex.specularMapON);
		glm::vec3 col = mMaterials[matID].color;
		glUniform3f(shader->GetUniformLocation("uColor"), col.r, col.g, col.b);
	}
	if (mat.normalMap >= 0)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, mat.normalMap);
		glUniform1i(shader->GetUniformLocation(("bumpMap")), 2);
	}
	else {
		glActiveTexture(GL_TEXTURE0 + 2);
		ResourceManager &resmgr = ResourceManager::getInstance();
		glBindTexture(GL_TEXTURE_2D, resmgr.flatNormalMap);
		glUniform1i(shader->GetUniformLocation(("bumpMap")), 2);
	}

	if (mat.specularMap >= 0)
	{
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, mat.specularMap);
		glUniform1i(shader->GetUniformLocation(("specularMap")), 3);
	}
	else {
		glActiveTexture(GL_TEXTURE0 + 3);
		ResourceManager &resmgr = ResourceManager::getInstance();
		glBindTexture(GL_TEXTURE_2D, resmgr.defaultSpecularMap);
		glUniform1i(shader->GetUniformLocation(("specularMap")), 3);
	}

	if (mat.emissiveMap >= 0)
	{
		glUniform1i(shader->GetUniformLocation(("uv_maps.emissiveMapON")), 1);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, mat.emissiveMap);
		glUniform1i(shader->GetUniformLocation(("emissiveMap")), 4);
	}
	else {
		glUniform1i(shader->GetUniformLocation(("uv_maps.emissiveMapON")), 0);
		glUniform1f(shader->GetUniformLocation(("uEmissive")), mat.emissiveFactor);
	}
}
void SkinnedMeshRenderer::Render(const std::unordered_map<btCollisionObject*, char>& collisionObjectArray, Shader *shader, char vtxattr, bool useMaterials)
{
	if (collisionObjectArray.find(mFrustumCollisionObject) == collisionObjectArray.end())
		return;
	// Draw the character only
	glBindVertexArray(m_VAO);

	const GLint POSITION_LOCATION = shader->GetAttributeLocation("in_position");
	glBindBuffer(GL_ARRAY_BUFFER, m_TFVBO);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);
	if (vtxattr & NORMAL_VTXATTR)
	{
		const GLint NORMAL_LOCATION = shader->GetAttributeLocation("in_normal");
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(3 * sizeof(GLfloat)));
	} 
	if (vtxattr & TANGENT_VTXATTR)
	{
		const GLint TANGENT_LOCATION = shader->GetAttributeLocation("in_tangent");
		glEnableVertexAttribArray(TANGENT_LOCATION);
		glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(6 * sizeof(GLfloat)));
	}
	if (vtxattr & TEXCOORD_VTXATTR)
	{
		const GLint TEX_COORD_LOCATION = shader->GetAttributeLocation("TexCoord");
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(9 * sizeof(GLfloat)));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	//glDrawArrays(GL_TRIANGLES, 0, NumIndices);

	for (uint i = 0; i < m_Entries.size(); i++)
	{
		if (useMaterials)
			UseMaterial(shader, m_Entries[i].MaterialIndex);
		glDrawElementsBaseVertex(GL_TRIANGLES,
		m_Entries[i].NumIndices,
		GL_UNSIGNED_INT,
		(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
		m_Entries[i].BaseVertex);
	}
	//if (useMaterials)
	//	UseMaterial(shader, 0);
	//glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	if (mCPUskinning && (vtxattr & TANGENT_VTXATTR))
	{

		glBindVertexArray(m_clothVAO);
		const GLint POSITION_LOCATION = shader->GetAttributeLocation("in_position");
		const GLint TEX_COORD_LOCATION = shader->GetAttributeLocation("TexCoord");
		const GLint NORMAL_LOCATION = shader->GetAttributeLocation("in_normal");
		const GLint TANGENT_LOCATION = shader->GetAttributeLocation("in_tangent");
		const GLint BONE_ID_LOCATION = shader->GetAttributeLocation("BoneIDs");
		const GLint BONE_WEIGHT_LOCATION = shader->GetAttributeLocation("Weights");
		
		glUniformMatrix4fv(shader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mWorldMatrix));

		glUniform1i(shader->GetUniformLocation(("uv_maps.diffuseMapON")), 0);
		glUniform1i(shader->GetUniformLocation(("uv_maps.specularMapON")), 0);
		glUniform3f(shader->GetUniformLocation(("uColor")), 0.04, 0.04, 0.05);

		glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_POS_VB]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(mSkinnedClothPositions[0]) * mSkinnedClothPositions.size(), &mSkinnedClothPositions[0], GL_STATIC_DRAW);	
		//int bfsize;
		//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bfsize);
		
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (vtxattr & NORMAL_VTXATTR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_NORMAL_VB]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(mSkinnedClothNormals[0]) * mSkinnedClothNormals.size(), &mSkinnedClothNormals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(NORMAL_LOCATION);
			glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if(vtxattr & TANGENT_VTXATTR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TANGENT_VB]);
			glEnableVertexAttribArray(TANGENT_LOCATION);
			glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (vtxattr & TEXCOORD_VTXATTR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TEXCOORD_VB]);
			glEnableVertexAttribArray(TEX_COORD_LOCATION);
			glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ClothBuffers[CLOTH_INDEX_BUFFER]);//??????

		if (useMaterials)
			UseMaterial(shader, clothMaterialID);

		glDrawElements(GL_TRIANGLES, mClothIndices.size(), GL_UNSIGNED_INT, 0);
		glCullFace(GL_FRONT);
		glUniform3f(shader->GetUniformLocation(("uColor")), 0.35, 0.35, 0.35);
		glDrawElements(GL_TRIANGLES, mClothIndices.size(),GL_UNSIGNED_INT,0);
		glCullFace(GL_BACK);

		//for (uint i = 0; i < m_Entries.size(); i++)
		//{
		//	glDrawElementsBaseVertex(GL_TRIANGLES,
		//		m_Entries[i].NumIndices,
		//		GL_UNSIGNED_INT,
		//		(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
		//		m_Entries[i].BaseVertex);
		//}
	}
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::RegisterSkeletalAnim(const aiNode* pNode, SkelTreeAnimTrack *pRoot)
{
	std::string NodeName(pNode->mName.data);

	aiAnimation* pAnimation = m_pScene->mAnimations[0];
	
	std::vector<aiNodeAnim*> pNodeAnims = FindNodeAnim(pAnimation, pNode->mName.data);
	
	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()){
		pRoot->m_boneIdx = m_BoneMapping[NodeName];
		if (pNodeAnims.size() > 0)
		{
		//	pRoot->m_name = NodeName;
		//	pRoot->m_staticTransf = convertMatrix4x4(pNode->mTransformation);
			for (uint a = 0; a < pNodeAnims.size(); a++) {
				aiNodeAnim* pNodeAnim = pNodeAnims[a];
				pRoot->m_translationKeys.resize(pNodeAnim->mNumPositionKeys);

				m_maxT = pNodeAnim->mNumPositionKeys / 24.f;
				for (int i = 0; i < pNodeAnim->mNumPositionKeys; i++)
				{
					const aiVector3D v = pNodeAnim->mPositionKeys[i].mValue;
					pRoot->m_translationKeys[i] = glm::vec3(v.x, v.y, v.z);
				}

				pRoot->m_rotationKeys.resize(pNodeAnim->mNumRotationKeys);
				for (int i = 0; i < pNodeAnim->mNumRotationKeys; i++)
				{
					const aiQuaternion q = pNodeAnim->mRotationKeys[i].mValue;
					pRoot->m_rotationKeys[i] = glm::quat(q.w, q.x, q.y, q.z);
				}

				pRoot->m_scalingKeys.resize(pNodeAnim->mNumScalingKeys);
				for (int i = 0; i < pNodeAnim->mNumScalingKeys; i++)
				{
					const aiVector3D v = pNodeAnim->mScalingKeys[i].mValue;
					pRoot->m_scalingKeys[i] = glm::vec3(v.x, v.y, v.z);
				}
			}
			for (uint i = 0; i < pNode->mNumChildren; i++)
			{
				if (m_BoneMapping.find(std::string(pNode->mChildren[i]->mName.data)) != m_BoneMapping.end())
				{
					pRoot->m_children.push_back(new SkelTreeAnimTrack());
					RegisterSkeletalAnim(pNode->mChildren[i], pRoot->m_children[pRoot->m_children.size() - 1]);
				}
			}
		}
		else{
			for (uint i = 0; i < pNode->mNumChildren; i++)
			{
				if (m_BoneMapping.find(std::string(pNode->mChildren[i]->mName.data)) != m_BoneMapping.end())
				{
					pRoot->m_children.push_back(new SkelTreeAnimTrack());
					RegisterSkeletalAnim(pNode->mChildren[i], pRoot->m_children[pRoot->m_children.size() - 1]);
				}
			}
		}
	}
	else
	{
		for (uint i = 0; i < pNode->mNumChildren; i++)
		{	
			RegisterSkeletalAnim(pNode->mChildren[i], pRoot);	
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::InitPose(const SkelTreeAnimTrack *p_rootBone, SkelTreePose *p_RootPose)
{
	p_RootPose->m_boneIdx = p_rootBone->m_boneIdx;
	glm::mat4 m = m_BoneInfo[p_RootPose->m_boneIdx].BoneOffset;
	glm::vec3 v = p_rootBone->m_translationKeys[0];
	p_RootPose->m_translation = glm::vec3(v.x,v.y,v.z);// glm::vec3(-m[3][0], -m[3][1], -m[3][2]);
	for (uint16_t i = 0; i < p_rootBone->m_children.size(); i++)
	{
		p_RootPose->m_children.resize(p_rootBone->m_children.size());
		p_RootPose->m_children[i] = new SkelTreePose;
		InitPose(p_rootBone->m_children[i], p_RootPose->m_children[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::BlendWithAnimAt(const SkelTreeAnimTrack *p_animaRef, const float &t, const float &blendFactor)
{
	float actualT = t;

	float expr = (float)p_animaRef->m_translationKeys.size() / 24.0;
	while (actualT - expr >= 0.001)
	{
		actualT = actualT - expr;
	}
	
	if (actualT < 0.002f)
		actualT = 0.0f;

	/*
	// optimization 2a
	if (blendFactor <= 0.001f)
	{
		return;
	}
	// optimization 1
	if (blendFactor >= 0.99)
	{
		SetAnimationAt(p_animaRef, actualT);
		return;
	}
	*/
	this->m_animaBoneStruct = (SkelTreeAnimTrack *)p_animaRef;
	//clock_t tstart = clock();
	BlendIntoPose(actualT, blendFactor);
	//printf("BLEND ______ INTO ______ POSE 2: %lf s\n", (double)(clock() - tstart) / CLOCKS_PER_SEC);//???????????????????????????????????????
}

//////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::SetAnimationAt(const SkelTreeAnimTrack *p_animaRef, const float &t)
{
	float actualT = t;
	float expr = (float)p_animaRef->m_translationKeys.size() / 24.0;
	while (actualT - expr >= 0.001)
	{
		actualT = actualT - expr;
	}
	if (actualT < 0.001f)
		actualT = 0.0f;

	this->m_animaBoneStruct = (SkelTreeAnimTrack *)p_animaRef;
	SetToPose(actualT);
}

//////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::FinishPosing(bool zeroRootTransform)
{
	//clock_t tstart = clock();
	AbsoluteTransformPose(m_rootPose);
	//printf("AbsoluteTransformPose: %lf s\n", (double)(clock() - tstart) / CLOCKS_PER_SEC);//???????????????????????????????????????
	//tstart = clock();

	if (!zeroRootTransform)
		SaveRootTransform();
	else
	{
		m_prevLeftHandTransform = m_crtLeftHandTransform = currentTransforms[leftHandID] * glm::inverse(m_BoneInfo[leftHandID].BoneOffset);
		m_prevRightHandTransform = m_crtRightHandTransform = currentTransforms[rightHandID] * glm::inverse(m_BoneInfo[rightHandID].BoneOffset);
		
		m_prevLeftAnkleTransform = m_crtLeftAnkleTransform = currentTransforms[leftAnkleID] * glm::inverse(m_BoneInfo[leftAnkleID].BoneOffset);
		m_prevRightAnkleTransform = m_crtRightAnkleTransform = currentTransforms[rightAnkleID] * glm::inverse(m_BoneInfo[rightAnkleID].BoneOffset);
		
		m_prevLeftShinTransform = m_crtLeftShinTransform = currentTransforms[leftShinID] * glm::inverse(m_BoneInfo[leftShinID].BoneOffset);
		m_prevRightShinTransform = m_crtRightShinTransform = currentTransforms[rightShinID] * glm::inverse(m_BoneInfo[rightShinID].BoneOffset);
		
		m_prevLeftForearmTransform = m_crtLeftForearmTransform = currentTransforms[leftForearmID] * glm::inverse(m_BoneInfo[leftForearmID].BoneOffset);
		m_prevRightForearmTransform = m_crtRightForearmTransform = currentTransforms[rightForearmID] * glm::inverse(m_BoneInfo[rightForearmID].BoneOffset);


		m_prevRootTransform = m_crtRootTransform = currentTransforms[mRootBoneID];
	}
	m_zeroTransform = zeroRootTransform;
}

////////////////////////////////////////////////////////////////////////////////////
//
void SkinnedMeshRenderer::SaveRootTransform(){
	m_prevRootTransform = m_crtRootTransform;
	m_crtRootTransform = currentTransforms[mRootBoneID];

	m_prevLeftHandTransform = m_crtLeftHandTransform;
	m_crtLeftHandTransform = currentTransforms[leftHandID] * glm::inverse(m_BoneInfo[leftHandID].BoneOffset);

	m_prevRightHandTransform = m_crtRightHandTransform;
	m_crtRightHandTransform= currentTransforms[rightHandID] * glm::inverse(m_BoneInfo[rightHandID].BoneOffset);

	m_prevLeftForearmTransform = m_crtLeftForearmTransform;
	m_crtLeftForearmTransform = currentTransforms[leftForearmID] * glm::inverse(m_BoneInfo[leftForearmID].BoneOffset);

	m_prevRightForearmTransform = m_crtRightForearmTransform;
	m_crtRightForearmTransform = currentTransforms[rightForearmID] * glm::inverse(m_BoneInfo[rightForearmID].BoneOffset);

	m_prevLeftAnkleTransform = m_crtLeftAnkleTransform;
	m_crtLeftAnkleTransform = currentTransforms[leftAnkleID] * glm::inverse(m_BoneInfo[leftAnkleID].BoneOffset);

	m_prevRightAnkleTransform = m_crtRightAnkleTransform;
	m_crtRightAnkleTransform = currentTransforms[rightAnkleID] * glm::inverse(m_BoneInfo[rightAnkleID].BoneOffset);

	m_prevLeftShinTransform = m_crtLeftShinTransform;
	m_crtLeftShinTransform = currentTransforms[leftShinID] * glm::inverse(m_BoneInfo[leftShinID].BoneOffset);

	m_prevRightShinTransform = m_crtRightShinTransform;
	m_crtRightShinTransform = currentTransforms[rightShinID] * glm::inverse(m_BoneInfo[rightShinID].BoneOffset);
}

//////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::SetToPose(const float animTime)
{
	m_tempAnimTime = animTime;
	m_rootPose->m_translation = CalcInterpolatedPosition(m_animaBoneStruct, m_tempAnimTime);
	m_rootPose->m_children[0]->m_translation = CalcInterpolatedPosition(m_animaBoneStruct->m_children[0], m_tempAnimTime);
	SetToPoseInternal(m_animaBoneStruct, m_rootPose);
}

void SkinnedMeshRenderer::SetToPoseInternal(const SkelTreeAnimTrack *p_rootBone, SkelTreePose *p_RootPose)
{
	//p_RootPose->m_translation = CalcInterpolatedPosition(p_rootBone, m_tempAnimTime);
	p_RootPose->m_rotation = CalcInterpolatedRotation(p_rootBone, m_tempAnimTime);
	//p_RootPose->m_scaling = CalcInterpolatedScaling(p_rootBone, m_tempAnimTime);

	for (uint16_t i = 0; i < std::min<unsigned>(p_rootBone->m_children.size(), p_RootPose->m_children.size()); i++)
	{
		SetToPoseInternal((const SkelTreeAnimTrack *)p_rootBone->m_children[i], p_RootPose->m_children[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::BlendIntoPose(const float animTime, const float blendFact)
{
	m_tempAnimTime = animTime;
	m_tempBlendFact = blendFact;

	m_tempRootPose->m_translation = CalcInterpolatedPosition(m_animaBoneStruct, m_tempAnimTime);
	m_tempRootPose->m_children[0]->m_translation = CalcInterpolatedPosition(m_animaBoneStruct->m_children[0], m_tempAnimTime);
	SetToPoseInternal(m_animaBoneStruct, m_tempRootPose);

	m_rootPose->m_translation = InterpolateTranslation(m_rootPose->m_translation, m_tempRootPose->m_translation, m_tempBlendFact);
	m_rootPose->m_children[0]->m_translation = InterpolateTranslation(m_rootPose->m_children[0]->m_translation, m_tempRootPose->m_children[0]->m_translation, m_tempBlendFact);
	BlendIntoPoseInternal(m_rootPose, m_tempRootPose);
}

void SkinnedMeshRenderer::BlendIntoPoseInternal(SkelTreePose *p_blendedPose, SkelTreePose *p_blenderPose)
{
	//p_blendedPose->m_translation = InterpolateTranslation(p_blendedPose->m_translation, (const glm::vec3)p_blenderPose->m_translation, m_tempBlendFact);
	p_blendedPose->m_rotation = InterpolateRotation(p_blendedPose->m_rotation, p_blenderPose->m_rotation, m_tempBlendFact);
	//p_blendedPose->m_scaling = InterpolateScaling(p_blendedPose->m_scaling, (const glm::vec3)p_blenderPose->m_scaling, m_tempBlendFact);

	for (uint16_t i = 0; i < p_blendedPose->m_children.size(); i++)
	{
		BlendIntoPoseInternal(p_blendedPose->m_children[i], p_blenderPose->m_children[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////


uint16_t __boneIdx;
glm::mat4 __localTransform;
glm::quat q;
float qxx, qyy, qzz, qxz, qxy, qyz, qwx, qwy, qwz;
/*
void SkinnedMeshRenderer::AbsoluteTransformPose(const SkelTreePose *p_rootBone, const glm::mat4 &parentTransform)
{
	__boneIdx = p_rootBone->m_boneIdx;
	q = p_rootBone->m_rotation;

	//const glm::mat4 scaling = glm::scale(glm::mat4(1), p_rootBone->m_scaling);
	__localTransform = glm::mat4(1);
	qxx = q.x * q.x;
	qyy = q.y * q.y;
	qzz = q.z * q.z;
	qxz = q.x * q.z;
	qxy = q.x * q.y;
	qyz = q.y * q.z;
	qwx = q.w * q.x;
	qwy = q.w * q.y;
	qwz = q.w * q.z;

	__localTransform[0][0] = 1 - 2 * (qyy + qzz);
	__localTransform[0][1] = 2 * (qxy + qwz);
	__localTransform[0][2] = 2 * (qxz - qwy);

	__localTransform[1][0] = 2 * (qxy - qwz);
	__localTransform[1][1] = 1 - 2 * (qxx + qzz);
	__localTransform[1][2] = 2 * (qyz + qwx);

	__localTransform[2][0] = 2 * (qxz + qwy);
	__localTransform[2][1] = 2 * (qyz - qwx);
	__localTransform[2][2] = 1 - 2 * (qxx + qyy);
	
	__localTransform[3][0] = p_rootBone->m_translation.x;
	__localTransform[3][1] = p_rootBone->m_translation.y;
	__localTransform[3][2] = p_rootBone->m_translation.z;

	const glm::mat4  __nodeTransform = parentTransform * __localTransform;

	currentTransforms[__boneIdx] = __nodeTransform * m_BoneInfo[__boneIdx].BoneOffset;
	
	for (uint16_t i = 0, sz = p_rootBone->m_children.size(); i < sz; i++)
	{
		AbsoluteTransformPose(p_rootBone->m_children[i], __nodeTransform);
	}
}

*/
#include <queue>

void SkinnedMeshRenderer::AbsoluteTransformPose(const SkelTreePose *p_rootBone)
{
	std::queue<std::pair<SkelTreePose*, glm::mat4> > boneQue;

	glm::mat4 parentTransform = glm::mat4(1);
	SkelTreePose *crtBone = (SkelTreePose*)p_rootBone;
	boneQue.push(std::pair<SkelTreePose*, glm::mat4>(crtBone, parentTransform));
	
	
	while(!boneQue.empty())
	{
		 crtBone = boneQue.front().first;
		 parentTransform = boneQue.front().second;
		 boneQue.pop();

		__boneIdx = crtBone->m_boneIdx;
		q = crtBone->m_rotation;

		//const glm::mat4 scaling = glm::scale(glm::mat4(1), p_rootBone->m_scaling);
		__localTransform = glm::mat4(1);
		qxx = q.x * q.x;
		qyy = q.y * q.y;
		qzz = q.z * q.z;
		qxz = q.x * q.z;
		qxy = q.x * q.y;
		qyz = q.y * q.z;
		qwx = q.w * q.x;
		qwy = q.w * q.y;
		qwz = q.w * q.z;

		__localTransform[0][0] = 1 - 2 * (qyy + qzz);
		__localTransform[0][1] = 2 * (qxy + qwz);
		__localTransform[0][2] = 2 * (qxz - qwy);

		__localTransform[1][0] = 2 * (qxy - qwz);
		__localTransform[1][1] = 1 - 2 * (qxx + qzz);
		__localTransform[1][2] = 2 * (qyz + qwx);

		__localTransform[2][0] = 2 * (qxz + qwy);
		__localTransform[2][1] = 2 * (qyz - qwx);
		__localTransform[2][2] = 1 - 2 * (qxx + qyy);

		__localTransform[3][0] = crtBone->m_translation.x;
		__localTransform[3][1] = crtBone->m_translation.y;
		__localTransform[3][2] = crtBone->m_translation.z;

		const glm::mat4  __nodeTransform = parentTransform * __localTransform;

		currentTransforms[__boneIdx] = __nodeTransform * m_BoneInfo[__boneIdx].BoneOffset;
	
		for (uint16_t i = 0; i < crtBone->m_children.size(); i++)
		{
			boneQue.push(std::pair<SkelTreePose*, glm::mat4>(crtBone->m_children[i], __nodeTransform));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool SkinnedMeshRenderer::InitFromScene(const aiScene* pScene, const char *fileName)
{
	LoadBonesHierarchy(m_pScene->mRootNode, false);
	for (auto s : m_clothBonesNames)
	{
		m_BoneMapping[s] += m_NumBones;
	}
	dualQuaternions.resize(m_NumBones + m_NumClothBones);
	currentTransforms.resize(m_NumBones + m_NumClothBones);
	m_BoneInfo.resize(m_NumBones + m_NumClothBones);

	int clothMeshID = -1;
	for (uint i = 0; i < pScene->mNumMeshes; i++) {
		if (pScene->mMeshes[i]->mNumBones > 128)
		{
			clothMeshID = i;
			mCPUskinning = true;
			
			break;
		}
	}
	m_Entries.resize(pScene->mNumMeshes - (mCPUskinning ? 1 : 0));
	//	m_Textures.resize(pScene->mNumMaterials);

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals, Tangents;
	std::vector<glm::vec2> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<uint> Indices;

	NumVertices = 0;
	NumIndices = 0;

	// Count the number of vertices and indices
	int k = 0;
	for (uint i = 0; i < pScene->mNumMeshes; i++) {
		if (pScene->mMeshes[i]->mNumBones > 128)
		{
			clothMaterialID = pScene->mMeshes[i]->mMaterialIndex;
			continue;
		}
		m_Entries[k].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[k].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[k].BaseVertex = NumVertices;
		m_Entries[k].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[k].NumIndices;
		k++;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	Tangents.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (int i = 0; i < pScene->mNumMeshes; i++) {
		if (mCPUskinning)
		{
			if (i == clothMeshID)
				continue;
			else if (i > clothMeshID)
				k = i - 1;
			else k = i;
		}
		else
		{
			k = i;
		}
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals,Tangents, TexCoords, Bones, Indices);
		for (uint b = 0; b < NumVertices; b++) {
			Bones[b].cleanup();
		}
	}




	if (mCPUskinning)
	{
		


		const aiMesh* paiMesh = pScene->mMeshes[clothMeshID];
		NumClothVertices = paiMesh->mNumVertices;
		
		mClothPositions.reserve(NumClothVertices);
		mClothNormals.reserve(NumClothVertices);
		mClothTangents.reserve(NumClothVertices);
		mClothTexCoords.reserve(NumClothVertices);
		mClothBones.resize(NumClothVertices);
		mClothIndices.reserve(paiMesh->mNumFaces * 3);

		mSkinnedClothPositions.resize(NumClothVertices);
		mSkinnedClothNormals.resize(NumClothVertices);
		mSkinnedClothTangents.resize(NumClothVertices);
		InitMesh(-1, paiMesh, mClothPositions, mClothNormals, mClothTangents, mClothTexCoords, mClothBones, mClothIndices);
		
		for (uint b = 0; b < NumClothVertices; b++)
		{
			mClothBones[b].cleanup();
		}
		glGenVertexArrays(1, &m_clothVAO);
		glBindVertexArray(m_clothVAO);
		glGenBuffers(NUM_CLOTH_VBs, m_ClothBuffers);
		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mClothPositions[0]) * mClothPositions.size(), &mClothPositions[0][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mClothTexCoords[0]) * mClothTexCoords.size(), &mClothTexCoords[0][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mClothNormals[0]) * mClothNormals.size(), &mClothNormals[0][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_ClothBuffers[CLOTH_TANGENT_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mClothTangents[0]) * mClothTangents.size(), &mClothTangents[0][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ClothBuffers[CLOTH_INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mClothIndices[0]) * mClothIndices.size(), &mClothIndices[0], GL_STATIC_DRAW);
	}
	//TEXTURES HERE
	if (!InitMaterials(pScene)) {
		return false;
	}

	glBindVertexArray(m_VAO);

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_TFVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TFVBO);
	glBufferData(GL_ARRAY_BUFFER, 11 * sizeof(GLfloat) * NumVertices, nullptr, GL_STATIC_READ);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TFVBO);

	glBindVertexArray(0);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::InitMesh(int MeshIndex, const aiMesh* paiMesh, std::vector<glm::vec3> &Positions,
	std::vector<glm::vec3> &Normals, std::vector<glm::vec3> &Tangents, std::vector<glm::vec2> &TexCoords, std::vector<VertexBoneData> &Bones, std::vector<unsigned int>& Indices) {
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (uint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTgt = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		Tangents.push_back(glm::vec3(pTgt->x, pTgt->y, pTgt->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	
	LoadBoneWeights(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (uint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		//assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}

bool SkinnedMeshRenderer::comparatorFuncDES(aiVertexWeight i, aiVertexWeight j) { return i.mWeight > j.mWeight; }

////////////////////////////////////////////////////////////////////////////////
void printmat4x4(glm::mat4 &mat)
{
	printf("__________________________________________________\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n__________________________________________________\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}
void SkinnedMeshRenderer::LoadBoneWeights(int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones) {

	mRootBoneID = 0;
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) != m_BoneMapping.end())
		{
			// Allocate an index for a new bone
			BoneIndex = m_BoneMapping[BoneName];
			
			BoneInfo bi;
			m_BoneInfo[BoneIndex] = bi;
			m_BoneInfo[BoneIndex].BoneOffset = convertMatrix4x4(pMesh->mBones[i]->mOffsetMatrix);
			
			if (strstr(BoneName.c_str(), "Hips") != NULL){
				printf("[SKINNED MESH LOADER] : Loading %s...\n", m_pScene->mRootNode->mName.data);
				//printmat4x4(m_BoneInfo[BoneIndex].BoneOffset);
			}
			if (strstr(BoneName.c_str(), "LeftHand") != NULL)
				leftHandID = BoneIndex;
			if (strstr(BoneName.c_str(), "LeftForearmBase") != NULL)
				leftForearmID = BoneIndex;

			if (strstr(BoneName.c_str(), "RightHand") != NULL)
				rightHandID = BoneIndex;
			if (strstr(BoneName.c_str(), "RightForearmBase") != NULL)
				rightForearmID = BoneIndex;

			if (strstr(BoneName.c_str(), "LeftFoot") != NULL)
				leftAnkleID = BoneIndex;
			if (strstr(BoneName.c_str(), "LeftShinBase") != NULL)
				leftShinID = BoneIndex;

			if (strstr(BoneName.c_str(), "RightFoot") != NULL)
				rightAnkleID = BoneIndex;
			if (strstr(BoneName.c_str(), "RightShinBase") != NULL)
				rightShinID = BoneIndex;
		}
		
		
		std::vector<aiVertexWeight> sortedWeights;

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			sortedWeights.push_back(pMesh->mBones[i]->mWeights[j]);
		}

		std::sort(sortedWeights.begin(), sortedWeights.end(), myobject);

		uint baseVert = MeshIndex != -1 ? m_Entries[MeshIndex].BaseVertex : 0;
		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {	
			uint VertexID = baseVert + sortedWeights[j].mVertexId;
			float Weight = sortedWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
#pragma message "assert BoneIndex exists"
		}
	}
}


void SkinnedMeshRenderer::LoadBonesHierarchy(aiNode *node, bool foundRoot, int depth)
{
	std::string bname(node->mName.data);
	if(!foundRoot)
		foundRoot = strstr(bname.c_str(), "Reference") != NULL;

	if (depth == 1 && node->mNumChildren == 0)
	{
		m_BoneMapping[bname] = m_NumClothBones;
		m_clothBonesNames.push_back(bname);
		m_NumClothBones++;
		return;
	}
	else if (foundRoot)
	{
		m_BoneMapping[bname] = m_NumBones;
		m_NumBones++;
		depth++;
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadBonesHierarchy(node->mChildren[i], foundRoot, depth);
	}
}
////////////////////////////////////////////////////////////////////////////////

bool SkinnedMeshRenderer::InitMaterials(const aiScene* pScene)
{
	for (int i = 0; i < pScene->mNumMaterials; i++)
	{
		aiMaterial *mat = pScene->mMaterials[i];
		glm::vec3 col = glm::vec3(0);
		float eFact = 0;
		{
			aiColor4D color(0.f, 0.f, 0.f, 0.f);
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color);
			col = glm::vec3(color.r, color.g, color.b);
		}
		{
			aiColor4D color(0.f, 0.f, 0.f, 0.f);
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color);
			eFact = dot(glm::vec3(0.299, 0.587, 0.114), glm::vec3(color.r, color.g, color.b));
		}
		Material m;

		int texcount = mat->GetTextureCount(aiTextureType_DIFFUSE);
		if (texcount > 0) {
			aiString tpath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &tpath);
			m.diffuseMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_COMPRESSED_RGBA);// _S3TC_DXT3_EXT);
		}
		texcount = mat->GetTextureCount(aiTextureType_NORMALS);
		if (texcount > 0) {
			aiString tpath;
			mat->GetTexture(aiTextureType_NORMALS, 0, &tpath);
			m.normalMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_RGB8);
		}

		texcount = mat->GetTextureCount(aiTextureType_SPECULAR);
		if (texcount > 0) {
			aiString tpath;
			mat->GetTexture(aiTextureType_SPECULAR, 0, &tpath);
			m.specularMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_COMPRESSED_RGB);// _S3TC_DXT1_EXT);
		}

		texcount = mat->GetTextureCount(aiTextureType_EMISSIVE);
		if (texcount > 0) {
			aiString tpath;
			mat->GetTexture(aiTextureType_EMISSIVE, 0, &tpath);
			m.emissiveMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_COMPRESSED_RED);
		}


		m.color = glm::vec3(col);
		mMaterials[i] = m;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRenderer::Clear() {
	/*for (uint i = 0; i < m_Textures.size(); i++) {
	SAFE_DELETE(m_Textures[i]);
	}*/

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(NUM_VBs, m_Buffers);
	}

	if (m_TFVBO != 0) {
		glDeleteBuffers(1, &m_TFVBO);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}

	if (mCPUskinning)
	{
		glDeleteBuffers(NUM_CLOTH_VBs, m_ClothBuffers);
		glDeleteVertexArrays(1, &m_clothVAO);
	}
}

///////////////////////////////////////////////////////////////////////////////////
std::vector<aiNodeAnim*> SkinnedMeshRenderer::FindNodeAnim(const aiAnimation* pAnimation, const char* NodeName) {

	std::vector<aiNodeAnim*> ret;
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		//				if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
		if (!strcmp(pNodeAnim->mNodeName.data, NodeName)) {
			ret.push_back(pNodeAnim);
		}
	}
	return ret;
}