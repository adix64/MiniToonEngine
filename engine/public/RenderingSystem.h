#pragma once
#include <Rendering/Shader.h>

#include <Rendering/ShadowCubeMapFBO.h>
#include <Rendering/ShadowMapFBO.h>
#include <Component/Light.h>

//#include "lighting_technique.h"
#include <Component/Camera.h>
#include <GameObjects/Character.h>
#include <Component/MeshRenderer.h>
#include <list>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <Rendering/GBuffer.h>
#include <Component/Sprite.h>
#include <Rendering/ColorFBO.h>

#include "HUD.h"
#define EPS 0.0001f

using namespace std;

class RenderSystem
{
private:
	RenderSystem();
	// Stop the compiler from generating copy methods
	RenderSystem(RenderSystem const& copy);            // do NOT implement
	RenderSystem& operator=(RenderSystem const& copy); // do NOT implement
public:

	static RenderSystem& getInstance()
	{
		static RenderSystem instance;
		return instance;
	}
	// Main stuct for btDbvt handling
	struct	DbvtBroadphaseFrustumCulling : btDbvt::ICollide {
		btAlignedObjectArray<btCollisionObject*>* m_pCollisionObjectArray;
		short int m_collisionFilterMask;
		btCollisionObject* m_additionalCollisionObjectToExclude;	// Unused in this demo

		DbvtBroadphaseFrustumCulling(btAlignedObjectArray<btCollisionObject*>* _pArray = NULL)
			: m_pCollisionObjectArray(_pArray), m_collisionFilterMask(btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger), m_additionalCollisionObjectToExclude(NULL)
		{
		}
		void	Process(const btDbvtNode* node, btScalar depth) { Process(node); }
		void	Process(const btDbvtNode* leaf) {
			btBroadphaseProxy*	proxy = static_cast < btBroadphaseProxy*> (leaf->data);
			btCollisionObject* co = static_cast < btCollisionObject* > (proxy->m_clientObject);
			if ((proxy->m_collisionFilterGroup & m_collisionFilterMask) != 0 && co != m_additionalCollisionObjectToExclude) {
				m_pCollisionObjectArray->push_back(co);
			}
		}
	} g_DBFC;

public:
	vector<MeshRenderer*> mMeshes;
	vector<SkinnedMeshRenderer*> mSkinnedMeshes;

	ColorFBO *mMiniMapFBO;
	Sprite *mMiniMapSprite;
	MeshRenderer *sphereMesh, *lightConeMesh, *lightConeHaloMesh;
	float m_aspectRatio;
	vector<MeshRenderer *> skyboxComponents;
	vector<PointLight*> pointLights;
	vector<SpotLight*>spotLights;
	vector<DirectionalLight*> directionalLights;

	unordered_map<string, Shader*> shaders;

	Camera *mActiveCamera;
	Camera *mDefaultCamera;
	int currFace; bool inv = false;
	GLuint screenQuad_VAO, screenQuad_VBO, screenQuad_IBO;
	//TODO
	GLuint diffuseTex, worldPosTex, worldNormalTex, charactersTex;
	int diffuseTex_loc, worldPosTex_loc, worldNormalTex_loc, charactersTex_loc, depthTex_loc;
	int frameTex_loc, edgesTex_loc, postProcessTex_loc;
	GLuint mToOnLUT1Dtexture, mEnvLUT1Dtexture;

	std::vector<Character::MotionTrace*> mMotionTraces;
	//GLuint mHatchTex1, mHatchTex2, mHatchTex3;// , mHatchTex4, mHatchTex5;
		
	void SetActiveCamera(Camera *camera);
	void addSpotLight(SpotLight *light);
	void addPointLight(PointLight *light);
	void addDirectionalLight(DirectionalLight *light);
	void AddSkyBox(MeshRenderer *front);

	void Init();
	void LoadSphere();
	void initScreenQuad();
	void initScreenQuadShader();
	void reload();

//SYSTEM UPDATE FUNCTION
	void RenderScene();

//TRANSFORM FEEDBACK SKINNING
	void TransformFeedbackSkinning();

//HUD & UI RENDER FUNCTIONS >>>>>>>
	void SplashScreen();
	void RenderMiniMap();
	void RenderHUD();
//<<<<<<<< HUD & UI RENDER FUNCTIONS

//SHADOW MAPPING Passes >>>>>>>
	void GenerateStaticShadows();
	void PointShadowPass(int lID);
	void DirShadowPass(int lID);
	void SpotShadowPass(int lID);
//<<<<<< SHADOW MAPPING Passes

//DEFERRED RENDERING >>>>>>>>>>>
	void GBufferPass();
	void LightAccumulationPass();
	void FinalDeferredPass();
	
	//Light Accumulation Passes
	void PointLightsPass();
	void OmniStencilPass(int i, glm::mat4 &lightTransform, Shader *spShader);
	void PointLightPass(int i, glm::mat4 &lightTransform, Shader *plShader);
	void SpotLightsPass();
	void SpotStencilPass(int i, glm::mat4 &lightTransform, Shader *spShader);
	void SpotLightPass(int i, glm::mat4 &lightTransform, Shader *slShader);
	void DirectionalLightsPass();
	void DirectionalLightPass(int i, Shader *slShader);
//<<<<<<<<<<< DEFERRED RENDERING

//FORWARD PASSES >>>>>>>>>>
	void DrawSkybox();
	void DrawToonOutlines();
	void DebugDrawLights();
	void DebugDrawLimbPositions();
	void DrawBoomRays();
	void DrawLightHalos();
	//<<<<<<<<<< FORWARD PASSES

//POST PROCESSING PASSES >>>>>>>>>>
	void DrawSSReflections();
	void BloomPass();
	void PostProcessingPass();
//<<<<<<<<< POST PROCESSING PASSES
		
	
	~RenderSystem();
	
	void SetWindowResolution(int width, int height);

	void PerformFrustumCulling(glm::mat4 &viewProj);
	void FrustumCullLights();
	void InitFrustumCollisionWorld();
	
		
	//geometry fbuffer
	GBuffer gbuffer;
	//bloom(2 pass) fbuffer
	ColorFBO *outputFBO, *postProcessFBO;
	ColorFBO *ssrFBO, *ssrFBOaux;
	ColorFBO *bloomFBO1, *bloomFBO2;
	ColorFBO *outlineFBO1, *outlineFBO2;
	//shadow cube maps for static objs
	std::unordered_map<int, ShadowCubeMapFBO*> staticCubeShadowMapFBOs;
	std::unordered_map<int, ShadowMapFBO*> staticSpotShadowMapFBOs;
	//shadow cascades for all dir lights
	std::unordered_map<int, CascadedShadowMapFBO*> directionalShadowMaps;
	//buffers for multiple passes & reuse in light accum pass(shadow_i->light_i;shadow_i+1->light_i+1)
	ShadowCubeMapFBO *blurredCubeShadowMapFBO;
	ShadowMapFBO *cubeFaceShadowMapFBO, *cubeFaceShadowMapFBO2, 
					*cascadeShadowMapFBO,  *cascadeShadowMapFBO2;
//FRUSTUM CULLING 
	btDiscreteDynamicsWorld *m_collisionWorld;
	btAlignedObjectArray<btCollisionObject*> m_objectsInFrustum;
	std::unordered_map<btCollisionObject*, char> m_objectsInFrustumHash;
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
//ENDFRUSTUM

	HUD *mHUD;

	int m_windowWidth, m_windowHeight;
	int m_actualWidth, m_actualHeight;
	glm::mat4 projection_view_matrix;
	void SetBoomRay(std::string &name, int active, glm::vec3 &position = glm::vec3(0));
	std::unordered_map<std::string, std::pair<int, glm::vec3> > mBoomRays;
	Sprite *boomRaySprite;
};
