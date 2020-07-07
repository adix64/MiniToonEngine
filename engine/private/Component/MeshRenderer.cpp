#pragma once
#pragma once
#include <Component/MeshRenderer.h>
#include <vector>
#include <climits>
#include <Rendering/Texture.h>
#include <ResourceManager.h>
#include <RenderingSystem.h>
#include <GameObject.h>

MeshRenderer::MeshRenderer(const std::string& Filename, bool renderable)
{
	mRenderable = renderable;
	mFilename = Filename;
	mShadowCaster = true;
	mWireframeDisplay = false;
	mVAO = 0;
	memset(mVtxBuffers, 0, sizeof(mVtxBuffers[0])* NUM_VBs);
	m_pScene = NULL;
}

void MeshRenderer::Awake()
{
	LoadMesh(mFilename);
	RenderSystem &renderSys = RenderSystem::getInstance();
	if(mRenderable){
	if(!mIsSkybox)
		renderSys.mMeshes.push_back(this);
	else 
		renderSys.skyboxComponents.push_back(this);
	}
}

MeshRenderer::~MeshRenderer()
{
	Clear();
}

bool MeshRenderer::LoadMesh(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(NUM_VBs, mVtxBuffers);

	bool Ret = false;

	ResourceManager &resMgr = ResourceManager::getInstance();
	
	m_pScene = resMgr.mImporter.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (m_pScene)
	{
		Ret = InitFromScene(m_pScene, Filename);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), resMgr.mImporter.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	return Ret;

}


void MeshRenderer::Render(Shader *shader, char vtxattr, bool useMaterials)
{
	glBindVertexArray(mVAO);

	const GLint POSITION_LOCATION = shader->GetAttributeLocation("in_position");
	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[POS_VB]);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (vtxattr & NORMAL_VTXATTR)
	{
		const GLint NORMAL_LOCATION = shader->GetAttributeLocation("in_normal");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[NORMAL_VB]);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	if (vtxattr & TANGENT_VTXATTR)
	{
		const GLint TANGENT_LOCATION = shader->GetAttributeLocation("in_tangent");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TANGENT_VB]);
		glEnableVertexAttribArray(TANGENT_LOCATION);
		glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	}
	if (vtxattr & TEXCOORD_VTXATTR)
	{
		const GLint TEX_COORD_LOCATION = shader->GetAttributeLocation("TexCoord");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TEXCOORD_VB]);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	//CheckOpenGLError2();
	for (uint i = 0; i < mMeshEntries.size(); i++)
	{
		//CheckOpenGLError2();
		const uint MaterialIndex = mMeshEntries[i].MaterialIndex;
		//CheckOpenGLError2();
		if(useMaterials)
			UseMaterial(shader, mMeshEntries[i].MaterialIndex);
		glDrawElementsBaseVertex(GL_TRIANGLES,
			mMeshEntries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * mMeshEntries[i].BaseIndex),
			mMeshEntries[i].BaseVertex);
		//CheckOpenGLError2();
	}
	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}

void MeshRenderer::RenderFrustumCollisionObjects(const std::unordered_map<btCollisionObject*, char>& collisionObjectArray, Shader *shader, char vtxattr, bool useMaterials)
{

	glBindVertexArray(mVAO);

	const GLint POSITION_LOCATION = shader->GetAttributeLocation("in_position");
	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[POS_VB]);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (vtxattr & NORMAL_VTXATTR)
	{
		const GLint NORMAL_LOCATION = shader->GetAttributeLocation("in_normal");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[NORMAL_VB]);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	if (vtxattr & TANGENT_VTXATTR)
	{
		const GLint TANGENT_LOCATION = shader->GetAttributeLocation("in_tangent");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TANGENT_VB]);
		glEnableVertexAttribArray(TANGENT_LOCATION);
		glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	}
	if (vtxattr & TEXCOORD_VTXATTR)
	{
		const GLint TEX_COORD_LOCATION = shader->GetAttributeLocation("TexCoord");
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TEXCOORD_VB]);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	//CheckOpenGLError2();
	//int drawObjects = 0;
	for (uint id = 0; id < mMeshEntries.size(); id++)
	{
		if (collisionObjectArray.find(mCollObjHash[id]) == collisionObjectArray.end())
			continue;
		//else drawObjects++;
		//CheckOpenGLError2();
		const uint MaterialIndex = mMeshEntries[id].MaterialIndex;
		//CheckOpenGLError2();
		if (useMaterials)
			UseMaterial(shader, mMeshEntries[id].MaterialIndex);
		glDrawElementsBaseVertex(GL_TRIANGLES,
			mMeshEntries[id].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * mMeshEntries[id].BaseIndex),
			mMeshEntries[id].BaseVertex);
		//CheckOpenGLError2();
	}
	//printf("drawObjects %d\n", drawObjects);
	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}

bool MeshRenderer::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	mMeshEntries.resize(pScene->mNumMeshes);
	//	m_Textures.resize(pScene->mNumMaterials);

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> Tangents;
	std::vector<glm::vec2> TexCoords;
	std::vector<uint> Indices;
	
	uint NumVertices = 0;
	uint NumIndices = 0;

	// Count the number of vertices and indices
	for (uint i = 0; i < mMeshEntries.size(); i++)
	{
		mMeshEntries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		mMeshEntries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		mMeshEntries[i].BaseVertex = NumVertices;
		mMeshEntries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += mMeshEntries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	Tangents.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (uint i = 0; i < mMeshEntries.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, Tangents, TexCoords, Indices);
	}

	//TEXTURES HERE
	if (!InitMaterials(pScene, Filename)) {
		return false;
	}

	// Generate and populate the buffers with vertex attributes and the indices

	//GLint POSITION_LOCATION = m_shader->GetAttributeLocation("in_position");
	//GLint TEX_COORD_LOCATION = m_shader->GetAttributeLocation("TexCoord");
	//GLint NORMAL_LOCATION = m_shader->GetAttributeLocation("in_normal");

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0][0], GL_STATIC_DRAW);
	//glEnableVertexAttribArray(POSITION_LOCATION);
	//glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0][0], GL_STATIC_DRAW);
	//glEnableVertexAttribArray(TEX_COORD_LOCATION);
	//glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0][0], GL_STATIC_DRAW);
	//glEnableVertexAttribArray(NORMAL_LOCATION);
	//glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVtxBuffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}

MeshRenderer::MeshRenderer(std::vector<VertexFormat> &vertices, std::vector<unsigned> &Indices, bool renderable)
{
	mRenderable = renderable;
	if (!InitFromData(vertices, Indices))
		printf("Error: could not load Mesh from raw data...");
}

bool MeshRenderer::InitFromData(std::vector <VertexFormat> &vertices, std::vector<unsigned> &Indices)
{
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> Tangents;
	std::vector<glm::vec2> TexCoords;
	for (int i = 0; i < vertices.size(); i++)
	{
		Positions.push_back(glm::vec3(vertices[i].position_x, vertices[i].position_y, vertices[i].position_z));
		Normals.push_back(glm::vec3(vertices[i].normal_x, vertices[i].normal_y, vertices[i].normal_z));
		Tangents.push_back(glm::vec3(vertices[i].tangent_x, vertices[i].tangent_y, vertices[i].tangent_z));
		TexCoords.push_back(glm::vec2(vertices[i].texcoord_x, vertices[i].texcoord_y));
	}


	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVtxBuffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	return true;
}

void MeshRenderer::FixedUpdate()
{
	for (uint i = 0, sz = mMeshEntries.size(); i < sz; i++)
	{
		for (char j = 0; j < 8; j++)
		{
			const glm::mat4 &modelMatrix = mGameObject->mTransform.GetWorldMatrix();
			mMeshEntries[i].worldSpaceAABB[j] = glm::vec3(modelMatrix * glm::vec4(mMeshEntries[i].objSpaceAABB[j], 1));
		}
	}
	//std::vector<Camera *> cams;
	//for (auto it = camerasUpdateRevisions.begin(); it != camerasUpdateRevisions.end(); ++it)
	//	cams.push_back(it->first);
	//for (uint i = 0, sz = cams.size(); i < sz; i++)
	//{
	//	camerasUpdateRevisions[cams[i]] = std::pair<long long, bool>(camerasUpdateRevisions[cams[i]].first - 1, true);
	//}
	//updated = true;
}

void MeshRenderer::InitMesh(uint MeshIndex, const aiMesh* paiMesh, std::vector<glm::vec3> &Positions,
	std::vector<glm::vec3> &Normals, std::vector<glm::vec3> &Tangents, std::vector<glm::vec2> &TexCoords, std::vector<unsigned int>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	RenderSystem &RenderSystem = RenderSystem::getInstance();
	float minX = FLT_MAX, maxX = FLT_MIN,
		  minY = FLT_MAX, maxY = FLT_MIN,
		  minZ = FLT_MAX, maxZ = FLT_MIN;
		
	// Populate the vertex attribute vectors
	for (uint i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTangent = &(paiMesh->mTangents[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		
		if (pPos->x < minX)
			minX = pPos->x;
		else if (pPos->x > maxX)
			maxX = pPos->x;

		if (pPos->y < minY)
			minY = pPos->y;
		else if (pPos->y > maxY)
			maxY = pPos->y;

		if (pPos->z < minZ)
			minZ = pPos->z;
		else if (pPos->z > maxZ)
			maxZ = pPos->z;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		Tangents.push_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(minX, minY, minZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(maxX, minY, minZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(minX, maxY, minZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(maxX, maxY, minZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(minX, minY, maxZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(maxX, minY, maxZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(minX, maxY, maxZ));
	mMeshEntries[MeshIndex].objSpaceAABB.push_back(glm::vec3(maxX, maxY, maxZ));

	mMeshEntries[MeshIndex].worldSpaceAABB = mMeshEntries[MeshIndex].objSpaceAABB;
	
	if(mRenderable && !mIsSkybox){
		
		std::vector<glm::vec3> &points = mMeshEntries[MeshIndex].objSpaceAABB;
		btCollisionShape* groundShape = new btConvexHullShape((const btScalar *)(&(points[0][0])), points.size(), 3 * sizeof(btScalar));

		RenderSystem.m_collisionShapes.push_back(groundShape);

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

		RenderSystem.m_collisionWorld->addRigidBody(body);// , group, mask);
		
		mCollObjHash[MeshIndex] = (btCollisionObject*)body;
		mCollObjVec.push_back((btCollisionObject*)body);
	}
	

	// Populate the index buffer
	for (uint i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i];
		//assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}
void MeshRenderer::SetDbvtWorldTransform(btTransform &trans) {
	for (int it = 0; it < mCollObjVec.size(); it++)
	{
		((btRigidBody*)mCollObjVec[it])->getMotionState()->setWorldTransform(trans);
		((btRigidBody*)mCollObjVec[it])->setActivationState(ACTIVE_TAG);
		//((btRigidBody*)(it->second))->setActivationState(ACTIVE_TAG);
		//renderSys.m_collisionWorld->removeRigidBody((btRigidBody*)(it->second));
		//renderSys.m_collisionWorld->addRigidBody((btRigidBody*)(it->second));


		//colobj.second->upda(ACTIVE_TAG);
		//colobj.second->setActivationState(ACTIVE_TAG);
	}
}

void MeshRenderer::UseMaterial(Shader *shader, int materialID)
{
	Material &mat = mMaterials[materialID];
	if (mat.diffuseMap >= 0)
	{
		glUniform1i(shader->GetUniformLocation(("uv_maps.diffuseMapON")), 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, mat.diffuseMap);
		glUniform1i(shader->GetUniformLocation("diffuseMap"), 1);
	}
	else
	{
		glUniform1i(shader->GetUniformLocation(("uv_maps.diffuseMapON")), 0);
		glUniform3f(shader->GetUniformLocation("uColor"), mat.color.x, mat.color.y, mat.color.z);
	}
	if (mat.normalMap >= 0)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, mat.normalMap);
		glUniform1i(shader->GetUniformLocation(("bumpMap")), 2);
	}else{
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

bool MeshRenderer::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	for (int i = 0; i < pScene->mNumMaterials;i++)
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

		int texcount =	mat->GetTextureCount(aiTextureType_DIFFUSE);
		if (texcount > 0){
			aiString tpath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &tpath);
			m.diffuseMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_COMPRESSED_RGBA);// _S3TC_DXT3_EXT);
		}
		texcount = mat->GetTextureCount(aiTextureType_NORMALS);
		if (texcount > 0) {
			aiString tpath;
			mat->GetTexture(aiTextureType_NORMALS, 0, &tpath);
			m.normalMap = Texture::png_texture_load(tpath.data, NULL, NULL, GL_RGB);
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
		m.emissiveFactor = eFact;
		mMaterials[i] = m;
	
	}

	return true;
}

void MeshRenderer::Clear()
{
	RenderSystem &RenderSystem = RenderSystem::getInstance();
	
	for (auto rb : mCollObjHash)
	{
		RenderSystem.m_collisionWorld->removeRigidBody((btRigidBody*)rb.first);
	}
	/*for (uint i = 0; i < m_Textures.size(); i++) {
	SAFE_DELETE(m_Textures[i]);
	}*/

	if (mVtxBuffers[0] != 0)
	{
		glDeleteBuffers(NUM_VBs, mVtxBuffers);
	}

	if (mVAO != 0)
	{
		glDeleteVertexArrays(1, &mVAO);
		mVAO = 0;
	}
}