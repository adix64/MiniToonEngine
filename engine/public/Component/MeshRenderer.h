#pragma once
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
#include <Rendering/Shader.h>
#include <Component/Camera.h>
#include <Component/Component.h>
#include "../../../dependencies/bullet3-2.86.1/src/btBulletCollisionCommon.h"
#include "../../../dependencies/bullet3-2.86.1/src/btBulletDynamicsCommon.h"


#define NORMAL_VTXATTR 1
#define TANGENT_VTXATTR 2
#define TEXCOORD_VTXATTR 4

typedef glm::mat2x4 DualQuaternion;

enum VB_TYPES {
	INDEX_BUFFER,
	POS_VB,
	NORMAL_VB,
	TANGENT_VB,
	TEXCOORD_VB,
	NUM_VBs
};

class MeshRenderer : public Component
{
public:
	struct MeshEntry
	{
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = 0;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;

		std::vector<glm::vec3> objSpaceAABB;
		std::vector<glm::vec3> worldSpaceAABB;
	};

	//for frustum culling -- keys:rigidbody, values:meshentryindex
	std::unordered_map<uint, btCollisionObject*> mCollObjHash;
	std::vector<btCollisionObject*> mCollObjVec;
	//render data
	std::unordered_map<int, Material> mMaterials;
	bool mShadowCaster = true;
	bool mIsSkybox = false;
	bool mRenderable;
	std::string mFilename;
	float depthOverride = 0.f;
	bool mWireframeDisplay = false;
	GLuint mVAO;
	GLuint mVtxBuffers[NUM_VBs];
	std::vector<MeshEntry> mMeshEntries;
	const aiScene* m_pScene;
		
		
	MeshRenderer(const std::string& Filename, bool renderable = true);
	MeshRenderer(std::vector<VertexFormat> &vertices, std::vector<unsigned> &indices, bool renderable = true);
	~MeshRenderer();
		
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(uint MeshIndex, const aiMesh* paiMesh, std::vector<glm::vec3> &Positions,
		std::vector<glm::vec3> &Normals, std::vector<glm::vec3> &Tangents, std::vector<glm::vec2> &TexCoords, std::vector<unsigned int>& Indices);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	bool InitFromData(std::vector<VertexFormat> &vertices, std::vector<unsigned> &indices);
	void Awake();
	void Clear();

	bool LoadMesh(const std::string& Filename);
	void setMaterial(Material &material) { mMaterials[mMeshEntries[0].MaterialIndex] = material;}

	void UseMaterial(Shader *shader, int matID);
	void RenderFrustumCollisionObjects(const std::unordered_map<btCollisionObject*, char>& collisionObjectArray, Shader *shader, char mode = NORMAL_VTXATTR | TANGENT_VTXATTR | TEXCOORD_VTXATTR, bool usematerials = false);
	void Render(Shader *shader, char mode = NORMAL_VTXATTR | TANGENT_VTXATTR | TEXCOORD_VTXATTR, bool useMaterials = false);
	void SetDbvtWorldTransform(btTransform &trans);
	void FixedUpdate();
	//void Update();
	//void LateUpdate();
};
