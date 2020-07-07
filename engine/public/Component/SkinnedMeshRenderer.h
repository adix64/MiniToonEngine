#pragma once
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
#include <Rendering/Shader.h>
#include <map>
#include <Component/CharacterAnimation/SkeletalAnimaPosing.h>
#include <Component/Component.h>
#include <btBulletCollisionCommon.h>
#define NORMAL_VTXATTR 1
#define TANGENT_VTXATTR 2
#define TEXCOORD_VTXATTR 4

struct SkelTreePose
{
	uint16_t m_boneIdx;
	glm::vec3 m_translation;
	glm::vec3 m_scaling;
	glm::quat m_rotation;
	std::vector<SkelTreePose *> m_children;

	SkelTreePose(){}
	
	~SkelTreePose()
	{
		for (int i = 0; i < m_children.size(); i++)
			if(m_children[i])
				delete m_children[i];
	}
};

class SkinnedMeshRenderer : public Component
{
public:
	
	SkinnedMeshRenderer(const char*Filename);

	~SkinnedMeshRenderer();
	///////////////// ANIMATION FUNCTIONS ///////////////////// 
	void SetAnimationAt(const SkelTreeAnimTrack *p_animaRef, const float &t);

	void BlendWithAnimAt(const SkelTreeAnimTrack *p_animaRef, const float &t, const float &blendFactor);

	void FinishPosing(bool zeroRootTransform);

	void RegisterSkeletalAnim(const aiNode* pNode, SkelTreeAnimTrack *);

	void InitPose(const SkelTreeAnimTrack *p_RootBone, SkelTreePose *p_RootPose);
	
	void SetToPose(const float animTime);

	void SetToPoseInternal(const SkelTreeAnimTrack *p_rootBone, SkelTreePose *p_RootPose);

	void BlendIntoPose(const float animTime, const float blendFact);

	void BlendIntoPoseInternal(SkelTreePose *p_blendedPose, SkelTreePose *p_blenderPose);

	void AbsoluteTransformPose(const SkelTreePose *p_rootBone);

	void SaveRootTransform();
	///////////////// ------------------- ////////////////////

	void prepareUniforms();

	void sendUniforms(Shader *shader);

	DualQuaternion QuatTrans2UDQ(const glm::quat &q0, const glm::vec3 &t);

	bool LoadMesh(const char *fileName);
	
	void Skin(Shader *transformFeedbackShader);

	void SkinOnCPU();
	void UseMaterial(Shader *shader, int matID);
	void Render(const std::unordered_map<btCollisionObject*, char>& collisionObjectArray, Shader *shader, char vtxattrs = NORMAL_VTXATTR | TANGENT_VTXATTR | TEXCOORD_VTXATTR, bool useMaterials = false);

	/*inline glm::mat4 GetCurrentInverseRootTransform() const
	{
		return glm::inverse(m_crtRootTransform);
	}*/

	inline glm::mat4 GetCurrentRootTransform() const
	{
		return m_crtRootTransform;
	}

	//inline glm::mat4 GetPrevInverseRootTransform() const
	//{
	//	return glm::inverse(m_prevRootTransform);
	//}

	//inline glm::mat4 GetPrevRootTransform() const
	//{
	//	return m_prevRootTransform;
	//}

	

//private:
#define NUM_BONES_PER_VERTEX 4

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		DualQuaternion dq;
		BoneInfo()
		{
			BoneOffset = glm::mat4(1);
		}
	};

	struct VertexBoneData
	{
		uint  IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		struct VertBoneInfo
		{
			float vWeight;
			uint vboneID;
			VertBoneInfo(uint p_vboneID, float p_vWeight)
			{
				vWeight = p_vWeight;
				vboneID = p_vboneID;
			}
		};

		struct compclass
		{
			bool operator() (VertBoneInfo a, VertBoneInfo b) { return (a.vWeight > b.vWeight); }
		} comparator;

		std::vector<VertBoneInfo> bonesInfo;
		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			memset(IDs, 0, sizeof(uint) * NUM_BONES_PER_VERTEX);
			memset(Weights, 0, sizeof(float) * NUM_BONES_PER_VERTEX);
		}


		void AddBoneData(uint BoneID, float Weight)
		{
			VertBoneInfo vbd(BoneID, Weight);
			bonesInfo.push_back(vbd);
		}

		void cleanup()
		{
			std::sort(bonesInfo.begin(), bonesInfo.end(), comparator);
			float unity = 0;
			int i = 0;
			for (i = 0; i < std::min((size_t)NUM_BONES_PER_VERTEX, bonesInfo.size()); i++)
			{
				//printf("[[[[[[[[[[[[[[[[ %d\n",bonesInfo.size());
				Weights[i] = bonesInfo[i].vWeight;
				unity += Weights[i];
				IDs[i] = bonesInfo[i].vboneID;
			}
			for (int j = 0; j < i; j++) {
				Weights[i] += (1.f - unity) / i;
			}
			bonesInfo.clear();
		}
	};


	std::vector<aiNodeAnim*> FindNodeAnim(const aiAnimation* pAnimation, const char* NodeName);

	void ReadNodeHeirarchy(const SkelTreeAnimTrack *p_RootBone, float AnimationTime, const glm::mat4 &ParentTransform);
				
	bool InitFromScene(const aiScene* pScene, const char *Filename);

	void InitMesh(int MeshIndex, const aiMesh* paiMesh, std::vector<glm::vec3> &Positions,
		std::vector<glm::vec3> &Normals, std::vector<glm::vec3> &Tangents, std::vector<glm::vec2> &TexCoords, std::vector<VertexBoneData> &Bones, std::vector<unsigned int>& Indices);

	bool comparatorFuncDES(aiVertexWeight i, aiVertexWeight j);
		
	struct myclass {
		bool operator() (aiVertexWeight i, aiVertexWeight j) { return (i.mWeight < j.mWeight); }
	} myobject;

	void LoadBoneWeights(int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
	void LoadBonesHierarchy(aiNode *node, bool foundRoot = false, int depth = 0);
	bool InitMaterials(const aiScene* pScene);
	
	void Clear();
		
#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES
	{
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TANGENT_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};
	GLuint NumVertices = 0;
	GLuint NumIndices = 0;
	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];
	GLuint m_TFVBO; // Transform feedback Vertex Buffer Object (character hardware skinning)
	
	enum CLOTH_VB_TYPES
	{
		CLOTH_INDEX_BUFFER,
		CLOTH_POS_VB,
		CLOTH_NORMAL_VB,
		CLOTH_TANGENT_VB,
		CLOTH_TEXCOORD_VB,
		NUM_CLOTH_VBs
	};
	GLuint NumClothVertices = 0;
	GLuint NumClothIndices = 0;
	GLuint m_clothVAO;
	GLuint m_ClothBuffers[NUM_CLOTH_VBs];

	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	glm::mat4 mWorldMatrix;

	std::unordered_map<int, Material> mMaterials;
	int clothMaterialID;
	std::vector<MeshEntry> m_Entries;
	std::map<std::string, uint> m_BoneMapping; // maps a bone name to its index
	uint m_NumBones, m_NumClothBones = 128;
	std::vector<std::string> m_clothBonesNames;
	std::vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;

	unsigned mRootBoneID, leftHandID, rightHandID, leftAnkleID, rightAnkleID, leftShinID, rightShinID, leftForearmID, rightForearmID;
	const aiScene* m_pScene;

	std::vector<glm::mat4> currentTransforms;
	std::vector<DualQuaternion> dualQuaternions;

	//glm::mat4 m_crtRootTransform;

	glm::mat4 m_crtRootTransform, m_prevRootTransform;

	glm::mat4 m_crtLeftHandTransform, m_prevLeftHandTransform;
	glm::mat4 m_crtRightHandTransform, m_prevRightHandTransform;

	glm::mat4 m_crtRightAnkleTransform, m_prevRightAnkleTransform;
	glm::mat4 m_crtLeftAnkleTransform, m_prevLeftAnkleTransform;

	glm::mat4 m_crtLeftForearmTransform, m_prevLeftForearmTransform;
	glm::mat4 m_crtRightForearmTransform, m_prevRightForearmTransform;
	
	glm::mat4 m_crtLeftShinTransform, m_prevLeftShinTransform;
	glm::mat4 m_crtRightShinTransform, m_prevRightShinTransform;

	SkelTreeAnimTrack *m_animaBoneStruct;
	SkelTreePose *m_rootPose;
	SkelTreePose *m_tempRootPose;
	float m_tempAnimTime, m_tempBlendFact;
	float m_maxT;
	bool m_zeroTransform;

	bool mCPUskinning = false;
	std::vector<glm::vec3> mClothPositions;
	std::vector<glm::vec3> mClothNormals,mClothTangents;

	std::vector<glm::vec3> mSkinnedClothPositions;
	std::vector<glm::vec3> mSkinnedClothNormals, mSkinnedClothTangents;

	std::vector<glm::vec2> mClothTexCoords;
	std::vector<VertexBoneData> mClothBones;
	std::vector<uint> mClothIndices;
	btCollisionObject *mFrustumCollisionObject;
};
