#pragma once
#include <Component/CharacterAnimation/SkeletalAnimaPosing.h>
#include <vector>
#include <iostream>
#include <map>

class SkeletalAnimation
{

public:

	SkeletalAnimation(const std::string& Filename);

	~SkeletalAnimation();
	///////////////// ANIMATION FUNCTIONS ///////////////////// 

	void RegisterSkeletalAnim(const aiNode* pNode, SkelTreeAnimTrack *, int framecount);

	bool LoadMesh(const std::string& Filename);

	//private:


	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		DualQuaternion dq;
		BoneInfo()
		{
			BoneOffset = glm::mat4(1);
		}
	};

	std::vector<aiNodeAnim*> FindNodeAnim(const aiAnimation* pAnimation, const char* NodeName);

	bool InitFromScene(const aiScene* pScene, const std::string& Filename);

	void LoadBones(uint MeshIndex, const aiMesh* pMesh);
	void LoadBonesHierarchy(aiNode *node, bool foundRoot);

	inline SkelTreeAnimTrack* GetAnimaBoneStruct() const { return m_animaBoneStruct; }

	uint m_NumBones;
	std::map<std::string, uint> m_BoneMapping; // maps a bone name to its index
	std::vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;
	unsigned hipsID, leftHandID, rightHandID;
	const aiScene* m_pScene;
	SkelTreeAnimTrack *m_animaBoneStruct;
	float m_maxTime;
};
