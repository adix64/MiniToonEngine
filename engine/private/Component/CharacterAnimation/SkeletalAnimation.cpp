#pragma once
#include <Component/CharacterAnimation/SkeletalAnimation.h>
#include <ResourceManager.h>
////////////////////////////////////////////////////////////////////////////////

SkeletalAnimation::SkeletalAnimation(const std::string& Filename)
{
	m_NumBones = 0;
	m_pScene = NULL;

	LoadMesh(Filename);
	m_animaBoneStruct = new SkelTreeAnimTrack();
	hipsID = leftHandID = rightHandID = UINT_MAX;
	RegisterSkeletalAnim(m_pScene->mRootNode, m_animaBoneStruct, -1);
}

////////////////////////////////////////////////////////////////////////////////

SkeletalAnimation::~SkeletalAnimation() {
	delete m_animaBoneStruct;
}

////////////////////////////////////////////////////////////////////////////////

bool SkeletalAnimation::LoadMesh(const std::string& Filename)
{
	bool Ret = false;
	ResourceManager &resMgr = ResourceManager::getInstance();
	m_pScene = resMgr.mImporter.ReadFile(Filename.c_str(), 0);

	if (m_pScene) {
		m_GlobalInverseTransform = convertMatrix4x4(m_pScene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), resMgr.mImporter.GetErrorString());
	}
	return Ret;
}

////////////////////////////////////////////////////////////////////////////////

void SkeletalAnimation::RegisterSkeletalAnim(const aiNode* pNode, SkelTreeAnimTrack *pRoot, int framecount)
{
	//if (framecount == -1)
	//{//unititialized 
		//framecount = -2;
	//}

	//std::string NodeName(pNode->mName.data);
	if (m_BoneMapping.find(pNode->mName.data) == m_BoneMapping.end())
	{//is NOT bone case
		for (uint i = 0; i < pNode->mNumChildren; i++)
		{
			RegisterSkeletalAnim(pNode->mChildren[i], pRoot, -1);
		}
		return;
	}

	

	aiAnimation* pAnimation = m_pScene->mAnimations[0];

	pRoot->m_boneIdx = m_BoneMapping[pNode->mName.data];

	uint animIdx = 0;
	bool foundAnimation = false;
	while (animIdx < m_pScene->mNumAnimations)
	{
		pAnimation = m_pScene->mAnimations[animIdx];

		//if (animIdx >= 1) {
			//int kk = -1;
		//}
		//else if (animIdx == 0) {
			//int kk = -1;
		//}
		std::vector<aiNodeAnim*> pNodeAnims = FindNodeAnim(pAnimation, pNode->mName.data);
		if (pNodeAnims.size() > 0)
		{
			//if (pNodeAnims.size() > 1)
			//{
			//	int kk = -1;
			//}
			foundAnimation = true;
			for (uint a = 0; a < pNodeAnims.size(); a++) {
				const aiNodeAnim* pNodeAnim = pNodeAnims[a];

				if (pNodeAnim->mNumPositionKeys > pRoot->m_translationKeys.size())
				{
					//std::vector<glm::vec3> tmp(pRoot->m_translationKeys.begin(), pRoot->m_translationKeys.end());
					uint numPosKeys = pNodeAnim->mNumPositionKeys;
					pRoot->m_translationKeys.resize(numPosKeys);
					for (int i = 0; i < numPosKeys; i++)
					{
						const aiVector3D v = pNodeAnim->mPositionKeys[i].mValue;
						pRoot->m_translationKeys[i] = glm::vec3(v.x, v.y, v.z);
					}

					m_maxTime = (float)pNodeAnim->mNumPositionKeys / 24.0 - 1.0 / 24.0;//??????????????????????????????		
				}
				if (pNodeAnim->mNumRotationKeys > pRoot->m_rotationKeys.size())
				{
					uint numRotKeys = pNodeAnim->mNumRotationKeys;
					pRoot->m_rotationKeys.resize(numRotKeys);
					for (int i = 0; i < numRotKeys; i++)
					{
						const aiQuaternion q = pNodeAnim->mRotationKeys[i].mValue;
						pRoot->m_rotationKeys[i] = glm::quat(q.w, q.x, q.y, q.z);
					}
					m_maxTime = (float)pNodeAnim->mNumPositionKeys / 24.0 - 1.0 / 24.0;//??????????????????????????????		
				}
			}
			//pRoot->m_scalingKeys.resize(pNodeAnim->mNumScalingKeys);
			//for (int i = 0; i < pNodeAnim->mNumScalingKeys; i++)
			//{
			//	const aiVector3D v = pNodeAnim->mScalingKeys[i].mValue;
			//	pRoot->m_scalingKeys[i] = glm::vec3(v.x, v.y, v.z);
			//}
		}
			
		animIdx++;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
	{
		if (m_BoneMapping.find(std::string(pNode->mChildren[i]->mName.data)) != m_BoneMapping.end())
		{
			if (foundAnimation)
			{
				pRoot->m_children.push_back(new SkelTreeAnimTrack());
				RegisterSkeletalAnim(pNode->mChildren[i], pRoot->m_children[pRoot->m_children.size() - 1], pRoot->m_translationKeys.size());
			}
			else
			{
				RegisterSkeletalAnim(pNode->mChildren[i], pRoot, -1);
			}
			
		}
	}
		
}

////////////////////////////////////////////////////////////////////////////////

bool SkeletalAnimation::InitFromScene(const aiScene* pScene, const std::string& Filename) {

	// Initialize the meshes in the scene one by one
	LoadBonesHierarchy(m_pScene->mRootNode, false);
	for (uint i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		LoadBones(i, paiMesh);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////

void SkeletalAnimation::LoadBonesHierarchy(aiNode *node, bool foundRoot)
{
	std::string bname(node->mName.data);
	if (!foundRoot)
		foundRoot = strstr(bname.c_str(), "Reference") != NULL;

	if (foundRoot)
	{
		m_BoneMapping[bname] = m_NumBones;
		m_NumBones++;
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadBonesHierarchy(node->mChildren[i], foundRoot);
	}
}
void printglmmat4x4(glm::mat4 &mat)
{
	printf("__________________________________________________\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n[%7.2f %7.2f %7.2f%7.2f]\n__________________________________________________\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}
////////////////////////////////////////////////////////////////////////////////

void SkeletalAnimation::LoadBones(uint MeshIndex, const aiMesh* pMesh)
{
	std::string FalseBone(m_pScene->mRootNode->mName.data);
	for (uint i = 0; i < pMesh->mNumBones; i++)
	{
		uint BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);
		if (FalseBone == BoneName)
			continue;
		if (m_BoneMapping.find(BoneName) != m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_BoneMapping[BoneName];

			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = convertMatrix4x4(pMesh->mBones[i]->mOffsetMatrix);
		
			if (strstr(BoneName.c_str(), "Hips") != NULL)
			{
				//printglmmat4x4(m_BoneInfo[BoneIndex].BoneOffset);
				hipsID = BoneIndex;
			}
			if (strstr(BoneName.c_str(), "RightHand") != NULL)
			{
				rightHandID = BoneIndex;
			}

			if (strstr(BoneName.c_str(), "RightHand") != NULL)
			{
				leftHandID = BoneIndex;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

std::vector<aiNodeAnim*> SkeletalAnimation::FindNodeAnim(const aiAnimation* pAnimation, const char* NodeName) {
	
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