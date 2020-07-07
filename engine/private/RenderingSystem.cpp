#pragma once
#include <RenderingSystem.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include "Rendering/glError.h"

#define OMNI_SHADOW_MAP_RES 768
#define DIRSHADOW_MAP_RES 768

#define AA_OMNI_SHADOW_MAP_RES 512
#define AA_DIRSHADOWMAP_RES 512

#define RES_DOWNSCALE_FACT 2.f/3.f
#define SSRFBOSCALE 1.f
using namespace std;
GLenum gCubeMapFaces[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

void RenderSystem::InitFrustumCollisionWorld(){
	///collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	m_broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;

	m_collisionWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

	m_collisionWorld->setGravity(btVector3(0, -10, 0));

	///create a few basic rigid bodies
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(5.), btScalar(50.)));

	const btAlignedObjectArray < btCollisionObject* >& collisionObjectArray = m_objectsInFrustum;
	

}

void RenderSystem::PerformFrustumCulling(glm::mat4 &viewProj)
{
	//return;
	auto start = SDL_GetPerformanceCounter();

	m_objectsInFrustum.resize(0);	// clear() is probably slower


	btDbvtBroadphase*	dbvtBroadphase = dynamic_cast <btDbvtBroadphase*> (m_collisionWorld->getBroadphase());

	// Storage stuff------------------
	btVector3 planes_n[5];
	btScalar  planes_o[5];
	//const btScalar farplane = frustumFar;
	static const int nplanes = sizeof(planes_n) / sizeof(planes_n[0]);
	static const bool cullFarPlane = true;//false;	// This can be tweaked.			
	const int	acplanes = cullFarPlane ? 5 : 4;
	
	float *mmp = &viewProj[0][0];	// mv*p
						// End storage stuff---------------
						// Frustum plane extraction--------
	{
		// Now we axtract the planes from mmp:

		// Extract the RIGHT clipping plane
		planes_n[0] = btVector3((btScalar)(mmp[3] - mmp[0]), (btScalar)(mmp[7] - mmp[4]), (btScalar)(mmp[11] - mmp[8]));
		// Extract the LEFT clipping plane
		planes_n[1] = btVector3((btScalar)(mmp[3] + mmp[0]), (btScalar)(mmp[7] + mmp[4]), (btScalar)(mmp[11] + mmp[8]));
		// Extract the TOP clipping plane
		planes_n[2] = btVector3((btScalar)(mmp[3] - mmp[1]), (btScalar)(mmp[7] - mmp[5]), (btScalar)(mmp[11] - mmp[9]));
		// Extract the BOTTOM clipping plane
		planes_n[3] = btVector3((btScalar)(mmp[3] + mmp[1]), (btScalar)(mmp[7] + mmp[5]), (btScalar)(mmp[11] + mmp[9]));
		// Extract the FAR clipping plane
		planes_n[4] = btVector3((btScalar)(mmp[3] - mmp[2]), (btScalar)(mmp[7] - mmp[6]), (btScalar)(mmp[11] - mmp[10]));
		//planes_n[4]	=	-dir;	//Should work well too... (don't know without normalizations... better stay coherent)

		// Extract the RIGHT clipping plane			
		planes_o[0] = (btScalar)(mmp[15] - mmp[12]);
		// Extract the LEFT clipping plane			
		planes_o[1] = (btScalar)(mmp[15] + mmp[12]);
		// Extract the TOP clipping plane			
		planes_o[2] = (btScalar)(mmp[15] - mmp[13]);
		// Extract the BOTTOM clipping plane			
		planes_o[3] = (btScalar)(mmp[15] + mmp[13]);
		// Extract the FAR clipping plane			
		planes_o[4] = (btScalar)(mmp[15] - mmp[14]);
	}
	// End Frustum plane extraction----

	//=======================================================
	// OK, now the pure btDbvt code starts here:
	//=======================================================
	g_DBFC.m_pCollisionObjectArray = &m_objectsInFrustum;
	g_DBFC.m_collisionFilterMask = btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger;	// This won't display sensors...
	g_DBFC.m_additionalCollisionObjectToExclude = NULL;//btCamera;

	btDbvt::collideKDOP(dbvtBroadphase->m_sets[1].m_root, planes_n, planes_o, acplanes, g_DBFC);
	btDbvt::collideKDOP(dbvtBroadphase->m_sets[0].m_root, planes_n, planes_o, acplanes, g_DBFC);
	// btDbvt::collideKDOP(root,normals,offsets,count,icollide): 
	// traverse the tree and call ICollide::Process(node) for all leaves located inside/on a set of planes.
	// End Perform Frustum Culling		
	auto end = SDL_GetPerformanceCounter();
	float timeTaken = (float)(end - start) / (float)SDL_GetPerformanceFrequency();

	m_objectsInFrustumHash.clear();
	for (int i = 0; i < m_objectsInFrustum.size(); i++)
		m_objectsInFrustumHash[m_objectsInFrustum[i]] = true;
}


bool generatedShadows = false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::AddSkyBox(MeshRenderer *dome)
{
	skyboxComponents.push_back(dome);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DrawSkybox()
{
	glEnable(GL_DEPTH_TEST);
	Shader *skyboxShader = shaders["skyboxShader"];
	skyboxShader->Use();
	glUniformMatrix4fv(skyboxShader->GetUniformLocation(("PVM")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix *
		glm::translate(glm::mat4(1), mActiveCamera->getPosition()) *
		glm::scale(glm::mat4(1), glm::vec3(10, 10, 10))));
	for (uint j = 0; j < skyboxComponents.size(); j++)
	{
		skyboxComponents[j]->Render(skyboxShader, TEXCOORD_VTXATTR, true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SetActiveCamera(Camera *camera) { this->mActiveCamera = camera; }
void RenderSystem::initScreenQuad()
{
	glGenVertexArrays(1, &screenQuad_VAO);
	glBindVertexArray(screenQuad_VAO);

	GLfloat vertices[] = { -1.f, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f, 1.f };
	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
	glGenBuffers(1, &screenQuad_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuad_VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &screenQuad_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenQuad_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glBindVertexArray(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::initScreenQuadShader()
{//init shading pass shader
#pragma message "optimize sending uniforms only once or delete this func"
	//return;
	shaders["pointLightShader"]->Use();
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("diffuseTex")), 2);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("worldPosTex")), 1);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("worldNormalTex")), 3);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("charactersTex")), 4);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("depthTex")), 5);

	//init postprocessing pass shader
	shaders["pointLightShader"]->Use();
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("wHeight")), m_actualHeight);

	shaders["postProcessShader"]->Use();
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("frameTex")), 6);
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("edgesTex")), 7);
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("postProcessTex")), 8);
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("wWidth")), m_windowWidth);
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("wHeight")), m_windowHeight);

	
}
RenderSystem::RenderSystem() {}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Init() {

	InitFrustumCollisionWorld();
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *extensions = glGetString(GL_EXTENSIONS);
	printf("System Info:\n\tVendor: %s\n\tRenderer: %s\n\tVersion: %s\n\n",
		vendor, renderer, version, extensions);

	m_actualWidth = 800; m_actualHeight = 450;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//forced wire, no illumination -> no surface shading


	{
		Shader *shader = new Shader("gpuSkinningShader");
		shader->AddShader("../shaders/gpu_skinning_vertex.glsl", GL_VERTEX_SHADER);
		const GLchar *varyingAttributes[] = { "world_pos", "world_normal", "world_tangent", "texcoord" };
		shader->CreateTransformFeedbackShader(4, varyingAttributes);
		shaders[shader->GetName()] = shader;
	}

	{//geometry pass shader
		Shader *shader = new Shader("gBufferPassShader");
		shader->AddShader("../shaders/geometry_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/geometry_pass_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//shadow map pass shader
		Shader *shader = new Shader("shadowMapShader");
		shader->AddShader("../shaders/shadow_map_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/shadow_map_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//null shader for stencil pass
		Shader *shader = new Shader("stencilPassShader");
		shader->AddShader("../shaders/light_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/null_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//point light shader
		Shader *shader = new Shader("pointLightShader");
		shader->AddShader("../shaders/light_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/pointLight_pass_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//point light shader
		Shader *shader = new Shader("spotLightShader");
		shader->AddShader("../shaders/light_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/spotLight_pass_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//directional light shader
		Shader *shader = new Shader("directionalLightShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/directionalLight_pass_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//final pass shaders
		Shader *shader = new Shader("finalPassShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/final_pass_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//post process shader
		Shader *shader = new Shader("postProcessShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/post_process_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//outline shader
		Shader *shader = new Shader("outlineShader");
		shader->AddShader("../shaders/outline_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/outline_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//screen space reflections shader
		Shader *shader = new Shader("ssReflectionsShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/screen_space_reflection.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//skybox shader
		Shader *shader = new Shader("skyboxShader");
		shader->AddShader("../shaders/skybox_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/flat_diffuse_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//static geo shader
		Shader *shader = new Shader("staticGeoShader");
		shader->AddShader("../shaders/static_geo_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/static_geo_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//Sprite shader
		Shader *shader = new Shader("spriteshader");
		shader->AddShader("../shaders/spriteVertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/flat_diffuse_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	
	{//VBlur shader SHADOW(greyscale, r component only)
		Shader *shader = new Shader("VshadowBlurShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/Vblur_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//HBlur shader SHADOW(greyscale, r component only)
		Shader *shader = new Shader("HshadowBlurShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/Hblur_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//VBlur shader RGB
		Shader *shader = new Shader("VrgbBlurShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/Vblur_fragmentRGB.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//HBlur shader RGB
		Shader *shader = new Shader("HrgbBlurShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/Hblur_fragmentRGB.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}


	{//bloom 1st pass shader
		Shader *shader = new Shader("bloomShader1");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/bloom1_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{//bloom 1st pass shader
		Shader *shader = new Shader("bloomShader2");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/bloom2_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{//minimap pass shader
		Shader *shader = new Shader("miniMapShader");
		shader->AddShader("../shaders/mini_map_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/mini_map_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	
	{////flat color shader
		Shader *shader = new Shader("flatColorShader");
		shader->AddShader("../shaders/flatColor_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/flatColor_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	
	{////light halo shader
		Shader *shader = new Shader("lightHaloShader");
		shader->AddShader("../shaders/light_halo_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/light_halo_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{////motionTraceShader
		Shader *shader = new Shader("motionTraceShader");
		shader->AddShader("../shaders/motion_trace_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/motion_trace_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{////sobel shader
		Shader *shader = new Shader("sobelShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/sobel_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{////luma shader
		Shader *shader = new Shader("lumaShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/luma_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{////fxaa shader
		Shader *shader = new Shader("fxaaShader");
		shader->AddShader("../shaders/fullscreen_pass_vertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("../shaders/fxaa_fragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	initScreenQuadShader();

	cubeFaceShadowMapFBO = new ShadowMapFBO();
	if (!cubeFaceShadowMapFBO->Init(OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES))
	{
		printf("Error initializing the shadow map FBO\n");
	}

	cubeFaceShadowMapFBO2 = new ShadowMapFBO();
	if (!cubeFaceShadowMapFBO2->Init(AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES))
	{
		printf("Error initializing the shadow map FBO\n");
	}

	cascadeShadowMapFBO = new ShadowMapFBO();
	if (!cascadeShadowMapFBO->Init(DIRSHADOW_MAP_RES, DIRSHADOW_MAP_RES))
	{
		printf("Error initializing the shadow map FBO\n");
	}

	cascadeShadowMapFBO2 = new ShadowMapFBO();
	if (!cascadeShadowMapFBO2->Init(AA_DIRSHADOWMAP_RES, AA_DIRSHADOWMAP_RES))
	{
		printf("Error initializing the shadow map FBO\n");
	}




	blurredCubeShadowMapFBO = new ShadowCubeMapFBO();
	if (!blurredCubeShadowMapFBO->Init(AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES))
	{
		printf("Error initializing the shadow map FBO\n");
	}

	outlineFBO1	= new ColorFBO();
	if (!outlineFBO1->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	outlineFBO2 = new ColorFBO();
	if (!outlineFBO2->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	bloomFBO1 = new ColorFBO();
	if (!bloomFBO1->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}
	bloomFBO2 = new ColorFBO();
	if (!bloomFBO2->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}

	ssrFBO = new ColorFBO();
	if (!ssrFBO->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight* SSRFBOSCALE, false, -1, GL_LINEAR_MIPMAP_LINEAR))
	{
		printf("Error initializing the screen space reflection FBO\n");
	}
	ssrFBOaux = new ColorFBO();
	if (!ssrFBOaux->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE, false, -1, GL_LINEAR_MIPMAP_LINEAR))
	{
		printf("Error initializing the aux screen space reflection FBO\n");
	}

	mDefaultCamera = new Camera(glm::vec3(5, 100, 5), glm::vec3(0,0,0), glm::vec3(0, 1, 0));
	///////////////////////////
	//TODO
	gbuffer.Init(m_actualWidth, m_actualHeight);
	outputFBO = new ColorFBO();
	outputFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, true, gbuffer.GetDepthTexture(), GL_LINEAR);

	postProcessFBO = new ColorFBO();
	postProcessFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR);

	initScreenQuad();
#pragma message "do i have to enable these?"
	//glEnable(GL_TEXTURE_CUBE_MAP);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);
	currFace = 0;

	m_windowWidth = 800; m_windowHeight = 450;
	m_aspectRatio = 16.f / 9.f;

	SetWindowResolution(m_windowWidth, m_windowHeight);

	LoadSphere();

	ResourceManager &resmgr = ResourceManager::getInstance();
	resmgr.LoadTextures();
	
	mHUD = new HUD();
	mHUD->Init();

	mMiniMapFBO = new ColorFBO();
	if (!mMiniMapFBO->Init(GL_RGBA8, 512, 512, true))
	{
		printf("Error initializing the bloom FBO\n");
	}
	mMiniMapSprite = new Sprite(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));
	mMiniMapSprite->texid = mMiniMapFBO->m_texture;
	mHUD->sprites["activeMap"] = mMiniMapSprite;
	int w, h;
	mToOnLUT1Dtexture = Texture::LUTtextureLoad("../assets/LUTs/ToonLUT.png", &w, &h);
	mEnvLUT1Dtexture = Texture::LUTtextureLoad("../assets/LUTs/EnvLUT.png", &w, &h);

	boomRaySprite = new Sprite(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));
	boomRaySprite->texid = resmgr.raysSprite;
}


void RenderSystem::SplashScreen()
{
	Shader *spriteshader = shaders["spriteshader"];
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	Sprite *sprite = new Sprite("../assets/splash2.png", glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));
	sprite->Render(spriteshader);
}

void RenderSystem::LoadSphere()
{
	sphereMesh = new MeshRenderer(std::string("../assets/uniformSphere.obj"), false);
	sphereMesh->Awake();

	lightConeMesh = new MeshRenderer(std::string("../assets/lightCone.obj"), false);
	lightConeMesh->Awake();

	lightConeHaloMesh = new MeshRenderer(std::string("../assets/lightConeHalo.obj"), false);
	lightConeHaloMesh->Awake();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SetWindowResolution(int width, int height)
{	//reshape
	if (height == 0)
		height = 1;

	for (auto it = shaders.begin(); it != shaders.end(); ++it)
	{
		it->second->uniformLocations.clear();
		it->second->attributeLocations.clear();
	}

	m_windowWidth = width;
	m_windowHeight = height;


	m_actualWidth = (float)width * RES_DOWNSCALE_FACT; m_actualHeight = (float)height * RES_DOWNSCALE_FACT;

	gbuffer.Init(m_actualWidth, m_actualHeight);
	outputFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, true, gbuffer.GetDepthTexture(), GL_LINEAR);

	postProcessFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR);

	shaders["postProcessShader"]->Use();
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("wWidth")), m_windowWidth);
	glUniform1i(shaders["postProcessShader"]->GetUniformLocation(("wHeight")), m_windowHeight);

	shaders["pointLightShader"]->Use();
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(shaders["pointLightShader"]->GetUniformLocation(("wHeight")), m_actualHeight);
	m_aspectRatio = (float)m_windowWidth / (float)m_windowHeight;

	if (!outlineFBO1->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	if (!outlineFBO2->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	if (!bloomFBO1->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}

	if (!bloomFBO2->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}
	if (!ssrFBO->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE, false, -1, GL_LINEAR))
	{
		printf("Error initializing the screen space reflection FBO\n");
	}
	if (!ssrFBOaux->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE, false, -1, GL_LINEAR))
	{
		printf("Error initializing the aux screen space reflection FBO\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::reload() {
	printf("Engine>> RELOADING SHADERS...\n");
	
	for (auto it = shaders.begin(); it != shaders.end(); it++)
	{
		it->second->Reload();
	}
	shaders.erase("gpuSkinningShader");
	{
		Shader *shader = new Shader("gpuSkinningShader");
		shader->AddShader("../shaders/gpu_skinning_vertex.glsl", GL_VERTEX_SHADER);
		const GLchar *varyingAttributes[] = { "world_pos", "world_normal", "world_tangent","texcoord" };
		shader->CreateTransformFeedbackShader(4, varyingAttributes);
		shaders[shader->GetName()] = shader;
	}
	printf("\n_______________________________________________\n");
	gbuffer.Init(m_actualWidth, m_actualHeight);
	outputFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, true, gbuffer.GetDepthTexture(), GL_LINEAR);

	postProcessFBO->Init(GL_RGB8, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR);

	if (!outlineFBO1->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	if (!outlineFBO2->Init(GL_RED, m_actualWidth, m_actualHeight, false, -1, GL_LINEAR))
	{
		printf("Error initializing the outline FBO\n");
	}

	if (!bloomFBO1->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}

	if (!bloomFBO2->Init(GL_RGB8, m_actualWidth / 2, m_actualHeight / 2, false, -1, GL_LINEAR))
	{
		printf("Error initializing the bloom FBO\n");
	}
	initScreenQuadShader();

	if (!ssrFBO->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE, false, -1, GL_LINEAR))
	{
		printf("Error initializing the screen space reflection FBO\n");
	}
	if (!ssrFBOaux->Init(GL_RGB8, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE, false, -1, GL_LINEAR))
	{
		printf("Error initializing the aux screen space reflection FBO\n");
	}
}

void RenderSystem::FrustumCullLights()
{

	int visibleSpotlights = 0;
	for (int i = 0; i < spotLights.size(); i++)
	{
		glm::vec3 sphereCenter = spotLights[i]->position;
		glm::mat4 invProjViewMat = glm::inverse(spotLights[i]->lightCam.projViewMat);
		std::vector<glm::vec3> verts;
		verts.push_back(sphereCenter);
		glm::vec4 v;
		v = invProjViewMat * glm::vec4(1, 1, 1, 1);
		verts.push_back(glm::vec3(v) / v.w);
		v = invProjViewMat * glm::vec4(1, -1, 1, 1);
		verts.push_back(glm::vec3(v) / v.w);
		v = invProjViewMat * glm::vec4(-1, 1, 1, 1);
		verts.push_back(glm::vec3(v) / v.w);
		v = invProjViewMat * glm::vec4(-1, -1, 1, 1);
		verts.push_back(glm::vec3(v) / v.w);

		if (HullInFrustum(verts, projection_view_matrix)){
			spotLights[i]->visible = true;
			visibleSpotlights++;
		}else
			spotLights[i]->visible = false;


	}

	int visiblePointlights = 0;
	for (int i = 0; i < pointLights.size(); i++)
	{
		glm::vec3 sphereCenter = pointLights[i]->position;
		float radius = pointLights[i]->scaleMat[0][0];
		
		if (SphereInFrustum(sphereCenter,radius , projection_view_matrix)) {
			pointLights[i]->visible = true;
			visiblePointlights++;
		}
		else
			pointLights[i]->visible = false;


	}
	//printf("Visible POINT lights %d\n", visiblePointlights);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::RenderScene()
{
	CheckOpenGLError();
	GTHTimes::TIME_renderTick();
	m_collisionWorld->stepSimulation(GTHTimes::TIME_renderTime);
	projection_view_matrix = mActiveCamera->getProjViewMatrix();
	
	TransformFeedbackSkinning();
	
	GenerateStaticShadows();
	
	gbuffer.StartFrame();
	
	GBufferPass();

	FrustumCullLights();

	LightAccumulationPass();

	FinalDeferredPass();

	DrawToonOutlines();	

	outputFBO->BindForWriting();
	DrawSkybox();

	//DebugDrawLights();
	DebugDrawLimbPositions();
	DrawBoomRays();
	DrawLightHalos();	

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	//DrawSSReflections();
	BloomPass();
	PostProcessingPass();

	RenderHUD();
	

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::TransformFeedbackSkinning()
{
	Shader *gpuSkShader = shaders["gpuSkinningShader"];
	gpuSkShader->Use();
	
	glEnable(GL_RASTERIZER_DISCARD);
	
	for (uint i = 0; i < mSkinnedMeshes.size(); i++) {
		if (!(mSkinnedMeshes[i]->mIsEnabled)) continue;
		//TODO
		mSkinnedMeshes[i]->prepareUniforms();
		
		mSkinnedMeshes[i]->sendUniforms(gpuSkShader);
		
		glUniformMatrix4fv(gpuSkShader->GetUniformLocation(("Model")),
			1, GL_FALSE, glm::value_ptr(mSkinnedMeshes[i]->mWorldMatrix));// mGameObject->mTransform.GetWorldMatrix()));
		
		mSkinnedMeshes[i]->Skin(gpuSkShader);
		
	}

	glDisable(GL_RASTERIZER_DISCARD);

}

void RenderSystem::DirShadowPass(int lID)
{

	Shader *smShader = shaders["shadowMapShader"];
	Shader *hBlurShader = shaders["HshadowBlurShader"];
	Shader *vBlurShader = shaders["VshadowBlurShader"];
	smShader->Use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	
	
	glDisable(GL_BLEND);
	
	directionalShadowMaps[lID]->Update(projection_view_matrix, -directionalLights[lID]->direction);
	
	for(int i = 0; i < 4; i++)
	{
		smShader->Use();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		cascadeShadowMapFBO->BindForWriting();
		
		glViewport(0, 0, DIRSHADOW_MAP_RES, DIRSHADOW_MAP_RES);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glm::mat4 shadow_proj_view = directionalShadowMaps[lID]->cascades.boxesViewProj[i];
		
		PerformFrustumCulling(shadow_proj_view);
		glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));

		
		

		for (uint j = 0; j < mMeshes.size(); j++) {
			if (mMeshes[j]->mShadowCaster)
			{
				glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->mGameObject->mTransform.GetWorldMatrix()));//**TODO MVP *optimization
				mMeshes[j]->RenderFrustumCollisionObjects(m_objectsInFrustumHash,smShader,0, false);
			}
		}
		for (uint j = 0; j < mSkinnedMeshes.size(); j++)
		{
			if (!(mSkinnedMeshes[j]->mIsEnabled)) continue;
			glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
			mSkinnedMeshes[j]->Render(m_objectsInFrustumHash, smShader, 0, false);
		}
		glDepthMask(GL_FALSE);
		
		//blur and output to cube map...
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glBindVertexArray(screenQuad_VAO);
		
		//VBLUR
		vBlurShader->Use();
		cascadeShadowMapFBO->BindForReading(GL_TEXTURE0);
		
		cascadeShadowMapFBO2->BindForWriting();
		
		glUniform1i(vBlurShader->GetUniformLocation("cubeShadowFace"), 0);
		
		glUniform1i(vBlurShader->GetUniformLocation("wWidth"), AA_DIRSHADOWMAP_RES);
		
		glUniform1i(vBlurShader->GetUniformLocation("wHeight"), AA_DIRSHADOWMAP_RES);
		
		glViewport(0, 0, AA_DIRSHADOWMAP_RES, AA_DIRSHADOWMAP_RES);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		//HBLUR
		hBlurShader->Use();
		
		cascadeShadowMapFBO2->BindForReading(GL_TEXTURE0);
		
		directionalShadowMaps[lID]->cascadeMaps[i]->BindForWriting();
		
		
		glUniform1i(hBlurShader->GetUniformLocation("cubeShadowFace"), 0);
		
		glUniform1i(hBlurShader->GetUniformLocation("wWidth"), AA_DIRSHADOWMAP_RES);
		glUniform1i(hBlurShader->GetUniformLocation("wHeight"), AA_DIRSHADOWMAP_RES);
		
		glViewport(0, 0, AA_DIRSHADOWMAP_RES, AA_DIRSHADOWMAP_RES);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		//...
	}
	
	////????????????????????????????????//--------------------------------------------------------------savdnvcxzxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SpotShadowPass(int lID)
{

	Shader *smShader = shaders["shadowMapShader"];
	Shader *hBlurShader = shaders["HshadowBlurShader"];
	Shader *vBlurShader = shaders["VshadowBlurShader"];
	smShader->Use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);


	glDisable(GL_BLEND);

	smShader->Use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	cubeFaceShadowMapFBO->BindForWriting();

	glViewport(0, 0, OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//float left = -30, right = 30, bottom = -30, top = 30, near = -30, far = 30;
	//glm::mat4 dsmproj = glm::ortho(left, right, bottom, top, near, far);
	//glm::mat4 dsmview = glm::lookAt(glm::vec3(0, 0, 0), -directionalLights[lID]->direction, glm::vec3(0, 1, 0));
	glm::mat4 shadow_proj_view = spotLights[lID]->lightCam.projViewMat;

	PerformFrustumCulling(shadow_proj_view);
	glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));

	//for (uint j = 0; j < mMeshes.size(); j++) {
	//	if (mMeshes[j]->mShadowCaster)
	//	{
	//		glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->GetModelMatrix()));//**TODO MVP *optimization
	//		mMeshes[j]->RenderFrustumCollisionObjects(m_objectsInFrustum, smShader, 0, false);
	//	}
	//}
	for (uint j = 0; j < mSkinnedMeshes.size(); j++)
	{
		if (!(mSkinnedMeshes[j]->mIsEnabled)) continue;
		//TODO frustum cull characters

		//if (!BoxInFrustum(mSkinnedMeshes[j]->ch_bounding_box->mMeshEntries[0].worldSpaceAABB, shadow_proj_view))
			//continue;
		glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
		mSkinnedMeshes[j]->Render(m_objectsInFrustumHash, smShader, 0, false);
#pragma "NO COLOR HERE!!!!"
	}
	glDepthMask(GL_FALSE);

	//blur and output to cube map...
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(screenQuad_VAO);
	//VBLUR
	vBlurShader->Use();
	cubeFaceShadowMapFBO->BindForReading(GL_TEXTURE0);
	cubeFaceShadowMapFBO2->BindForWriting();
	glUniform1i(vBlurShader->GetUniformLocation("cubeShadowFace"), 0);
	glUniform1i(vBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
	glUniform1i(vBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

	glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//HBLUR
	hBlurShader->Use();
	cubeFaceShadowMapFBO2->BindForReading(GL_TEXTURE0);
	cubeFaceShadowMapFBO->BindForWriting();
	glUniform1i(hBlurShader->GetUniformLocation("cubeShadowFace"), 0);
	glUniform1i(hBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
	glUniform1i(hBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

	glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	//...

	////????????????????????????????????//--------------------------------------------------------------savdnvcxzxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::PointShadowPass(int lID)
{
	
	Shader *smShader = shaders["shadowMapShader"];
	Shader *hBlurShader = shaders["HshadowBlurShader"];
	Shader *vBlurShader = shaders["VshadowBlurShader"];
	smShader->Use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	if (pointLights[lID]->mShadowsOn)
	{
		for (uint i = 0; i < 6; i++) {
			smShader->Use();
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			cubeFaceShadowMapFBO->BindForWriting();
			glViewport(0, 0, OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES);
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			glm::mat4 shadow_proj_view = pointLights[lID]->lightCams[i].getProjViewMatrix();

			glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));


			//for (uint j = 0; j < mMeshes.size(); j++) {
			//	if (mMeshes[j]->mShadowCaster)
			//	{
			//		glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->GetModelMatrix()));//**TODO MVP *optimization
			//		mMeshes[j]->Render(smShader, &(pointLights[lID]->lightCams[i]), false);
			//	}
			//}

			for (uint j = 0; j < mSkinnedMeshes.size(); j++)
			{
				if (!(mSkinnedMeshes[j]->mIsEnabled)) continue;
				//TODO frustum cull characters
				//if (!BoxInFrustum(mSkinnedMeshes[j]->ch_bounding_box->mMeshEntries[0].worldSpaceAABB, pointLights[lID]->lightCams[i].getViewFrustum()))
					//continue;
				glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
				mSkinnedMeshes[j]->Render(m_objectsInFrustumHash, smShader, 0, false);
#pragma "NO COLOR HERE!!!!"
			}
			//blur and output to cube map...
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glBindVertexArray(screenQuad_VAO);
			//VBLUR
			vBlurShader->Use();
			cubeFaceShadowMapFBO->BindForReading(GL_TEXTURE0);
			cubeFaceShadowMapFBO2->BindForWriting();
			glUniform1i(vBlurShader->GetUniformLocation("cubeShadowFace"), 0);
			glUniform1i(vBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
			glUniform1i(vBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

			glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//HBLUR
			hBlurShader->Use();
			cubeFaceShadowMapFBO2->BindForReading(GL_TEXTURE0);
			blurredCubeShadowMapFBO->BindForWriting(gCubeMapFaces[i]);
			glUniform1i(hBlurShader->GetUniformLocation("cubeShadowFace"), 0);
			glUniform1i(hBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
			glUniform1i(hBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

			glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			//...
		}
	}
////????????????????????????????????//--------------------------------------------------------------savdnvcxzxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::GBufferPass()
{
	gbuffer.GBufferPassInit();
	glDepthMask(GL_TRUE);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	/////////////////////eostufff
	Shader *gBuffShader = shaders["gBufferPassShader"];
	gBuffShader->Use();

	glViewport(0, 0, m_actualWidth, m_actualHeight);

	//glUniform1i(gBuffShader->GetUniformLocation(("mWireframeDisplay")), 0);

	glUniformMatrix4fv(gBuffShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
	PerformFrustumCulling(projection_view_matrix);
	glUniform1i(gBuffShader->GetUniformLocation(("toon_shading")), 0);

	for (int j = 0; j < mMeshes.size(); j++) {

		if (!mMeshes[j]->mIsEnabled)
			continue;
		//GPUTexture tx;// = mSkinnedMeshes[0]->texture;
		//setTexture(tx);
		//setTexture(mMeshes[j]->texture);
		glUniformMatrix4fv(gBuffShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->mGameObject->mTransform.GetWorldMatrix())); //TODO
		glUniform1i(gBuffShader->GetUniformLocation(("mWireframeDisplay")), mMeshes[j]->mWireframeDisplay);

		if (mMeshes[j]->mWireframeDisplay)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mMeshes[j]->RenderFrustumCollisionObjects(m_objectsInFrustumHash, gBuffShader, NORMAL_VTXATTR | TANGENT_VTXATTR | TEXCOORD_VTXATTR, true);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(gBuffShader->GetUniformLocation(("toon_shading")), 1);

	for (uint j = 0; j < mSkinnedMeshes.size(); j++)
	{
		if (!(mSkinnedMeshes[j]->mIsEnabled)) continue;
		//TODO frustum cull characters

		/*if (!BoxInFrustum(mSkinnedMeshes[j]->ch_bounding_box->mMeshEntries[0].worldSpaceAABB, camera->getViewFrustum()))
			continue;*/
		//		continue;
		//mSkinnedMeshes[j]->sendUniforms(shaders["gBufferPassShader"]);
		glUniformMatrix4fv(gBuffShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
		mSkinnedMeshes[j]->Render(m_objectsInFrustumHash, gBuffShader, NORMAL_VTXATTR | TANGENT_VTXATTR | TEXCOORD_VTXATTR, true);
	}
	//printf("Objs drawn %d\n", m_objectsInFrustum.size());
	glDepthMask(GL_FALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::LightAccumulationPass()
{
	gbuffer.LightPassInit();
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
	
	DirectionalLightsPass();
	
	PointLightsPass();
	
	SpotLightsPass();
	
}

//////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DirectionalLightsPass()
{
	Shader *dlShader = shaders["directionalLightShader"];
	dlShader->Use();
	
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetPositionsTexture());
	glUniform1i(dlShader->GetUniformLocation(("worldPosTex")), 1);
	
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(dlShader->GetUniformLocation(("worldNormalTex")), 2);
	
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(dlShader->GetUniformLocation(("charactersTex")), 3);
	
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetDepthTexture());
	glUniform1i(dlShader->GetUniformLocation(("depthTex")), 4);

	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_1D, mToOnLUT1Dtexture);
	glUniform1i(dlShader->GetUniformLocation("uToonLUT"), 12);

	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_1D, mEnvLUT1Dtexture);
	glUniform1i(dlShader->GetUniformLocation("uEnvLUT"), 11);
	
	glUniformMatrix4fv(dlShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
	
	glUniform1i(dlShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(dlShader->GetUniformLocation(("wHeight")), m_actualHeight);
	
	glUniform3f(dlShader->GetUniformLocation(("eye_position")), mActiveCamera->getPosition().x, mActiveCamera->getPosition().y, mActiveCamera->getPosition().z);
	
	for (int i = 0; i < directionalLights.size(); i++)
	{
		if(directionalLights[i]->mShadowsOn)
			DirShadowPass(i);
		
		DirectionalLightPass(i, dlShader);
		
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DirectionalLightPass(int i, Shader *dlShader)
{

	gbuffer.LightPassInit();

	dlShader->Use();
	glViewport(0, 0, m_actualWidth, m_actualHeight);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

#define SHADOW_TEXTURE_UNIT(k) (GL_TEXTURE0 + 5 + (k))
	if (directionalLights[i]->mShadowsOn)
	{
		glUniform1i(dlShader->GetUniformLocation(("shadows")), 1);
		
		char ttext[32];
		for (int j = 0; j < 4; j++)
		{
			memset(ttext, 0, 32);
			sprintf(ttext, "shadowMap%d", j );
			directionalShadowMaps[i]->cascadeMaps[j]->BindForReading(SHADOW_TEXTURE_UNIT(j));
			glUniform1i(dlShader->GetUniformLocation(ttext), SHADOW_TEXTURE_UNIT(j) - GL_TEXTURE0);
			memset(ttext, 0, 32);
			sprintf(ttext, "lightPV[%d]", j );
			glUniformMatrix4fv(dlShader->GetUniformLocation(ttext), 1, GL_FALSE, glm::value_ptr(directionalShadowMaps[i]->cascades.boxesViewProj[j]));
			
		
		}
	}
	else
	{
		glUniform1i(dlShader->GetUniformLocation(("shadows")), 0);
	}

	glUniform3f(dlShader->GetUniformLocation(("uLight.direction")), directionalLights[i]->direction.x, directionalLights[i]->direction.y, directionalLights[i]->direction.z);
	glUniform3f(dlShader->GetUniformLocation(("uLight.color")), directionalLights[i]->color.r, directionalLights[i]->color.g, directionalLights[i]->color.b);
	glUniform1f(dlShader->GetUniformLocation(("uLight.power")), directionalLights[i]->power);
	glUniform1f(dlShader->GetUniformLocation(("uLight.ambientIntensity")), directionalLights[i]->ambientIntensity);
	glUniform3f(dlShader->GetUniformLocation(("uLight.luminance")), directionalLights[i]->luminance, directionalLights[i]->luminance, directionalLights[i]->luminance);

	glBindVertexArray(screenQuad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::PointLightsPass()
{
	Shader *plShader = shaders["pointLightShader"];
	Shader *spShader = shaders["stencilPassShader"];
	plShader->Use();
	//gbuffer.LightPassInit();
	//glViewport(0, 0, m_actualWidth, m_actualHeight);
	//glClearColor(0, 0, 0, 1);

	//glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetPositionsTexture());
	glUniform1i(plShader->GetUniformLocation(("worldPosTex")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(plShader->GetUniformLocation(("worldNormalTex")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(plShader->GetUniformLocation(("charactersTex")), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_1D, mToOnLUT1Dtexture);
	glUniform1i(plShader->GetUniformLocation("uToonLUT"), 4);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_1D, mEnvLUT1Dtexture);
	glUniform1i(plShader->GetUniformLocation("uEnvLUT"), 5);


	glUniformMatrix4fv(plShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));

	glUniform1i(plShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(plShader->GetUniformLocation(("wHeight")), m_actualHeight);

	glUniform3f(plShader->GetUniformLocation(("eye_position")), mActiveCamera->getPosition().x, mActiveCamera->getPosition().y, mActiveCamera->getPosition().z);


	spShader->Use();
	glUniformMatrix4fv(spShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));


	for (int i = 0; i < pointLights.size(); i++)
	{
		if (!pointLights[i]->visible)
			continue;
		glm::mat4 lightTransform = glm::translate(glm::mat4(1), pointLights[i]->position) * pointLights[i]->scaleMat;
		PointShadowPass(i);
		glEnable(GL_STENCIL_TEST);
		gbuffer.LightPassInit();
		OmniStencilPass(i, lightTransform, spShader);
		PointLightPass(i, lightTransform, plShader);
		glDisable(GL_STENCIL_TEST);
	}

	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::OmniStencilPass(int i, glm::mat4 &lightTransform, Shader *spShader)
{
	glDrawBuffer(GL_NONE);

	spShader->Use();
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	// Disable color/depth write and enable stencil

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glUniformMatrix4fv(spShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightTransform));
	sphereMesh->Render(spShader, 0, false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::PointLightPass(int i, glm::mat4 &lightTransform, Shader *plShader)
{

	gbuffer.LightPassInit();

	plShader->Use();
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glUniformMatrix4fv(plShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightTransform));

#define SHADOW_TEXTURE_UNIT(k) (GL_TEXTURE0 + 6 + (k))
	if (pointLights[i]->mShadowsOn)
	{
		glUniform1i(plShader->GetUniformLocation(("shadows")), 1);
		blurredCubeShadowMapFBO->BindForReading(SHADOW_TEXTURE_UNIT(1), plShader->GetUniformLocation(("cubeShadowMap")));

		staticCubeShadowMapFBOs[i]->BindForReading(SHADOW_TEXTURE_UNIT(2), plShader->GetUniformLocation(("staticCubeShadowMap")));
	}
	else
	{
		glUniform1i(plShader->GetUniformLocation(("shadows")), 0);
	}

	glUniform3f(plShader->GetUniformLocation(("pointLight.position")), pointLights[i]->position.x, pointLights[i]->position.y, pointLights[i]->position.z);
	glUniform3f(plShader->GetUniformLocation(("pointLight.color")), pointLights[i]->color.r, pointLights[i]->color.g, pointLights[i]->color.b);
	glUniform3f(plShader->GetUniformLocation(("pointLight.attenuation")), pointLights[i]->attenuation.r, pointLights[i]->attenuation.g, pointLights[i]->attenuation.b);
	glUniform1f(plShader->GetUniformLocation(("pointLight.power")), pointLights[i]->power);
	glUniform1f(plShader->GetUniformLocation(("pointLight.ambientIntensity")), pointLights[i]->ambientIntensity);
	glUniform3f(plShader->GetUniformLocation(("pointLight.luminance")), pointLights[i]->luminance, pointLights[i]->luminance, pointLights[i]->luminance);

	sphereMesh->Render(plShader, 0, false);

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SpotStencilPass(int i, glm::mat4 &lightTransform, Shader *spShader)
{
	glDrawBuffer(GL_NONE);

	spShader->Use();
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	// Disable color/depth write and enable stencil

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glUniformMatrix4fv(spShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightTransform));
	sphereMesh->Render(spShader, 0, false);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SpotLightsPass()
{
	Shader *slShader = shaders["spotLightShader"];
	Shader *spShader = shaders["stencilPassShader"];
	slShader->Use();
	//gbuffer.LightPassInit();
	//glViewport(0, 0, m_actualWidth, m_actualHeight);
	//glClearColor(0, 0, 0, 1);

	//glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetPositionsTexture());
	glUniform1i(slShader->GetUniformLocation(("worldPosTex")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(slShader->GetUniformLocation(("worldNormalTex")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(slShader->GetUniformLocation(("charactersTex")), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_1D, mToOnLUT1Dtexture);
	glUniform1i(slShader->GetUniformLocation("uToonLUT"), 4);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_1D, mEnvLUT1Dtexture);
	glUniform1i(slShader->GetUniformLocation("uEnvLUT"), 5);

	glUniformMatrix4fv(slShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));

	glUniform1i(slShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(slShader->GetUniformLocation(("wHeight")), m_actualHeight);

	glUniform3f(slShader->GetUniformLocation(("eye_position")), mActiveCamera->getPosition().x, mActiveCamera->getPosition().y, mActiveCamera->getPosition().z);


	spShader->Use();
	glUniformMatrix4fv(spShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));



	for (int i = 0; i < spotLights.size(); i++)
	{
		if (!spotLights[i]->visible)
			continue;
		glm::mat4 lightTransform = spotLights[i]->mLightWorldMatrix;// glm::translate(glm::mat4(1), spotLights[i]->position) * spotLights[i]->scaleMat;
		SpotShadowPass(i);
		glEnable(GL_STENCIL_TEST);
		gbuffer.LightPassInit();
		SpotStencilPass(i, lightTransform, spShader);
		SpotLightPass(i, lightTransform, slShader);
		glDisable(GL_STENCIL_TEST);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::SpotLightPass(int i, glm::mat4 &lightTransform, Shader *slShader)
{

	gbuffer.LightPassInit();

	slShader->Use();
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glUniformMatrix4fv(slShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightTransform));



#define SHADOW_TEXTURE_UNIT(k) (GL_TEXTURE0 + 6 + (k))

	if (spotLights[i]->mShadowsOn)
	{
		glUniform1i(slShader->GetUniformLocation(("shadows")), 1);
	
		glUniformMatrix4fv(slShader->GetUniformLocation("lightPV"), 1, GL_FALSE, glm::value_ptr(spotLights[i]->lightCam.getProjViewMatrix()));
		cubeFaceShadowMapFBO2->BindForReading(SHADOW_TEXTURE_UNIT(1));//, slShader->GetUniformLocation(("shadowMap")));
		glUniform1i(slShader->GetUniformLocation("shadowMap"), SHADOW_TEXTURE_UNIT(1) - GL_TEXTURE0);

		staticSpotShadowMapFBOs[i]->BindForReading(SHADOW_TEXTURE_UNIT(2));
		glUniform1i(slShader->GetUniformLocation("staticShadowMap"), SHADOW_TEXTURE_UNIT(2) - GL_TEXTURE0);

	}
	else
	{
		glUniform1i(slShader->GetUniformLocation(("shadows")), 0);
	}

	//glUniform1i(shaders["spotLightshader"]->GetUniformLocation(("cubeShadowMap")), SHADOW_TEXTURE_UNIT(1) - GL_TEXTURE0);

	glUniform3f(slShader->GetUniformLocation(("spotLight.position")), spotLights[i]->position.x, spotLights[i]->position.y, spotLights[i]->position.z);
	glUniform3f(slShader->GetUniformLocation(("spotLight.direction")), spotLights[i]->direction.x, spotLights[i]->direction.y, spotLights[i]->direction.z);
	glUniform1f(slShader->GetUniformLocation(("spotLight.cutoff")), spotLights[i]->cutoff);
	glUniform1f(slShader->GetUniformLocation(("spotLight.blurCutoff")), spotLights[i]->blur_cutoff);
	glUniform3f(slShader->GetUniformLocation(("spotLight.direction")), spotLights[i]->direction.x, spotLights[i]->direction.y, spotLights[i]->direction.z);
	glUniform3f(slShader->GetUniformLocation(("spotLight.color")), spotLights[i]->color.r, spotLights[i]->color.g, spotLights[i]->color.b);
	glUniform3f(slShader->GetUniformLocation(("spotLight.attenuation")), spotLights[i]->attenuation.r, spotLights[i]->attenuation.g, spotLights[i]->attenuation.b);
	glUniform1f(slShader->GetUniformLocation(("spotLight.power")), spotLights[i]->power);
	glUniform1f(slShader->GetUniformLocation(("spotLight.ambientIntensity")), spotLights[i]->ambientIntensity);
	glUniform3f(slShader->GetUniformLocation(("spotLight.luminance")), spotLights[i]->luminance, spotLights[i]->luminance, spotLights[i]->luminance);

	lightConeMesh->Render(slShader, 0, false);

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float cumulTime = 0.0f;

void RenderSystem::DrawToonOutlines()
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_actualWidth, m_actualHeight);
	
	Shader *lumaShader = shaders["lumaShader"];
	lumaShader->Use();

	outlineFBO2->BindForWriting();
	glUniform1i(lumaShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(lumaShader->GetUniformLocation(("wHeight")), m_actualHeight);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(lumaShader->GetUniformLocation(("frameTex")), 1);

	glBindVertexArray(screenQuad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Shader *sobelShader = shaders["sobelShader"];
	sobelShader->Use();

	outlineFBO1->BindForWriting();

	glUniform1i(sobelShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(sobelShader->GetUniformLocation(("wHeight")), m_actualHeight);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, outlineFBO2->m_texture);
	glUniform1i(sobelShader->GetUniformLocation(("normalTex")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(sobelShader->GetUniformLocation(("charactersTex")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetDepthTexture());
	glUniform1i(sobelShader->GetUniformLocation(("depthTex")), 3);


	glBindVertexArray(screenQuad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

//	return;
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_DEPTH_TEST);
//	glDepthMask(GL_TRUE);
//
//	Shader *outlineShader = shaders["outlineShader"];
//	outlineShader->Use();
//	glUniformMatrix4fv(outlineShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
//	glm::vec3 eye = mActiveCamera->getPosition();
//	glUniform3f(outlineShader->GetUniformLocation(("eye_pos")), eye.x, eye.y, eye.z);
//	glCullFace(GL_FRONT);
//
//#pragma message "DON'T DO FRUSTUM CULLING TWICE, it's COSTLY"
//	PerformFrustumCulling(projection_view_matrix);
//	cumulTime += 2.1 * (1 + rand() % 2) * GTHTimes::TIME_renderTime;
//	glUniform1f(outlineShader->GetUniformLocation(("uTime")), cumulTime);
//	for (int i = 0; i < mSkinnedMeshes.size(); i++)
//	{
//		glCullFace(GL_FRONT);
//		if (!(mSkinnedMeshes[i]->mIsEnabled)) continue;
//		glUniformMatrix4fv(outlineShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
//		mSkinnedMeshes[i]->Render(m_objectsInFrustumHash, outlineShader, NORMAL_VTXATTR | TANGENT_VTXATTR, false);
//	}
//
//	glCullFace(GL_BACK);
//	glDisable(GL_CULL_FACE);//for skybox7
}

void RenderSystem::DrawLightHalos()
{
	////////////////DEBUG DRAW LIGHTS
	Shader *haloShader = shaders["lightHaloShader"];
	haloShader->Use();
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
		
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUniformMatrix4fv(haloShader->GetUniformLocation(("PV")), 1, GL_FALSE, 
						glm::value_ptr(projection_view_matrix));
	glUniform3f(haloShader->GetUniformLocation("eyePos"), mActiveCamera->position.x,
														  mActiveCamera->position.y,
														  mActiveCamera->position.z);
	for (int i = 0; i < spotLights.size(); i++)
	{
		glm::mat4 lightMatrix = spotLights[i]->mLightWorldMatrix;
		glUniform3f(haloShader->GetUniformLocation("uColor"), (spotLights[i]->color).x, (spotLights[i]->color).y, (spotLights[i]->color).z);
		glUniform3f(haloShader->GetUniformLocation("lightPos"), (spotLights[i]->position).x, (spotLights[i]->position).y, (spotLights[i]->position).z);
		glUniformMatrix4fv(haloShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
		//glCullFace(GL_FRONT);
		//lightConeHaloMesh->Render(haloShader, NORMAL_VTXATTR, false);
		//glCullFace(GL_BACK);
		lightConeHaloMesh->Render(haloShader, NORMAL_VTXATTR, false);
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	//DEBUG DRAW LIGHTS

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DebugDrawLimbPositions()
{
	//////////////////DEBUG DRAW LIGHTS
	//Shader *flatColorShader = shaders["flatColorShader"];
	//flatColorShader->Use();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glUniformMatrix4fv(flatColorShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
	//
	//for (int i = 0; i < mSkinnedMeshes.size(); i++)
	//{
	//	glUniform3f(flatColorShader->GetUniformLocation("uColor"), 1, 1, 0);
	//	glm::mat4 lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtLeftAnkleTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtRightAnkleTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	glUniform3f(flatColorShader->GetUniformLocation("uColor"), 1, 0, 0);
	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtLeftHandTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtRightHandTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	glUniform3f(flatColorShader->GetUniformLocation("uColor"), 1, 1, 1);
	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtLeftForearmTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtRightForearmTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	glUniform3f(flatColorShader->GetUniformLocation("uColor"), 0, 1, 1);
	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtLeftShinTransform;// * glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);

	//	lightMatrix = mSkinnedMeshes[i]->mWorldMatrix * mSkinnedMeshes[i]->m_crtRightShinTransform;// *glm::scale(glm::mat4(1), glm::vec3(3.f));
	//	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
	//	sphereMesh->Render(flatColorShader, 0, false);
	//}
	
	////////////////////////////////////////----------------------------//////////////////////////////////////////////
	Shader *mtShader = shaders["motionTraceShader"];
	mtShader->Use();
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniformMatrix4fv(mtShader->GetUniformLocation(("PV")), 1, GL_FALSE,
		glm::value_ptr(projection_view_matrix));

	for (int i = 0; i < mMotionTraces.size(); i++)
	{
		if (!mMotionTraces[i]->active) continue;
		glUniform3f(mtShader->GetUniformLocation("uColor"), (spotLights[i]->color).x, (spotLights[i]->color).y, (spotLights[i]->color).z);
		mMotionTraces[i]->Render(mtShader);
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RenderSystem::SetBoomRay(std::string &name, int active, glm::vec3 &position)
{
	mBoomRays[name] = std::pair<int, glm::vec3>(active, position);
}


void RenderSystem::DrawBoomRays()
{
	////////////////////////////////////////----------------------------//////////////////////////////////////////////
	Shader *mtShader = shaders["spriteshader"];
	mtShader->Use();
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glm::mat4 mvpMat;

	glm::vec3 axis = glm::vec3(0, 1, 0);
	float angle = 0; 
	if(mActiveCamera->forward != glm::vec3(0,0,1) && mActiveCamera->forward != glm::vec3(0, 0, -1))
	{
		axis = glm::cross(glm::vec3(0, 0, 1), mActiveCamera->forward);
		angle = glm::acos(glm::dot(glm::vec3(0, 0, 1), mActiveCamera->forward));
	}
	for (auto boomRay: mBoomRays)
	{
		if (boomRay.second.first == 1)
		{
			mvpMat = mActiveCamera->getProjViewMatrix() 
				* translate(glm::mat4(1), boomRay.second.second)
				* glm::rotate(glm::mat4(1),angle, axis)
				*glm::scale(glm::mat4(1), glm::vec3(2));
			boomRaySprite->mvpMatrix = mvpMat;
			boomRaySprite->Render(mtShader);
			mBoomRays[boomRay.first] = std::pair<int,glm::vec3>(2, boomRay.second.second);
		}
		else if (boomRay.second.first == 2)
		{
			mvpMat = mActiveCamera->getProjViewMatrix()
				* translate(glm::mat4(1), boomRay.second.second)
				* glm::rotate(glm::mat4(1), angle, axis);
				//*glm::scale(glm::mat4(1), glm::vec3(2));
			boomRaySprite->mvpMatrix = mvpMat;
			boomRaySprite->Render(mtShader);
			mBoomRays[boomRay.first] = std::pair<int, glm::vec3>(0, boomRay.second.second);
		}
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DebugDrawLights()
{
	////////////////DEBUG DRAW LIGHTS
	Shader *flatColorShader = shaders["flatColorShader"];
	flatColorShader->Use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniformMatrix4fv(flatColorShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
	glUniform3f(flatColorShader->GetUniformLocation("uColor"), 1, 0, 0);
	for (int i = 0; i < spotLights.size(); i++)
	{
		glm::mat4 lightMatrix = spotLights[i]->mLightWorldMatrix;
		glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
		lightConeMesh->Render(flatColorShader, 0, false);
	}

	for (int i = 0; i < pointLights.size(); i++)
	{
		glm::mat4 lightMatrix = glm::translate(glm::mat4(1), pointLights[i]->position) * pointLights[i]->scaleMat;
		glUniformMatrix4fv(flatColorShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(lightMatrix));
		sphereMesh->Render(flatColorShader, 0, false);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//DEBUG DRAW LIGHTS

}

///////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::DrawSSReflections()
{
	Shader *ssrShader = shaders["ssReflectionsShader"];
	ssrShader->Use();

	ssrFBO->BindForWriting();

	glUniform1i(ssrShader->GetUniformLocation(("wWidth")), m_actualWidth * SSRFBOSCALE);
	glUniform1i(ssrShader->GetUniformLocation(("wHeight")), m_actualHeight* SSRFBOSCALE);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, outputFBO->m_texture);
	glUniform1i(ssrShader->GetUniformLocation(("gFinalImage")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetPositionsTexture());
	glUniform1i(ssrShader->GetUniformLocation(("gPosition")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(ssrShader->GetUniformLocation(("gNormal")), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(ssrShader->GetUniformLocation(("gExtraComponents")), 4);
	
	glm::mat4 persp = mActiveCamera->getProjectionMatrix();
	glm::mat4 perspInv = glm::inverse(persp);
	glm::mat4 view = mActiveCamera->getViewMatrix();
	//glm::mat4 viewInv = glm::inverse(view);git
	glUniformMatrix4fv(ssrShader->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
	//glUniformMatrix4fv(ssrShader->GetUniformLocation("invView"), 1, GL_FALSE, glm::value_ptr(viewInv));
	glUniformMatrix4fv(ssrShader->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(ssrShader->GetUniformLocation("invprojection"), 1, GL_FALSE, glm::value_ptr(perspInv));

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glViewport(0, 0, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(screenQuad_VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays

	/////////////////////////////////////
	Shader *vBlurShader = shaders["VrgbBlurShader"];
	Shader *hBlurShader = shaders["HrgbBlurShader"];

		//blur and output to cube map...
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(screenQuad_VAO);
	//VBLUR
	vBlurShader->Use();
	ssrFBOaux->BindForWriting();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssrFBO->m_texture);
	glUniform1i(vBlurShader->GetUniformLocation("texture"), 0);
	glUniform1i(vBlurShader->GetUniformLocation("wWidth"), m_actualWidth * SSRFBOSCALE);
	glUniform1i(vBlurShader->GetUniformLocation("wHeight"), m_actualHeight * SSRFBOSCALE);

	glViewport(0, 0, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//HBLUR
	hBlurShader->Use();

	ssrFBO->BindForWriting();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssrFBOaux->m_texture);
	glUniform1i(hBlurShader->GetUniformLocation("texture"), 0);
	glUniform1i(hBlurShader->GetUniformLocation("wWidth"), m_actualWidth * SSRFBOSCALE);
	glUniform1i(hBlurShader->GetUniformLocation("wHeight"), m_actualHeight * SSRFBOSCALE);

	glViewport(0, 0, m_actualWidth * SSRFBOSCALE, m_actualHeight * SSRFBOSCALE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void RenderSystem::FinalDeferredPass()
{
	Shader *fpShader = shaders["finalPassShader"];
	fpShader->Use();
	//gbuffer.FinalPassInit();
	outputFBO->BindForWriting();
	//
	glUniform3f(fpShader->GetUniformLocation("eye_position"), mActiveCamera->position.x, mActiveCamera->position.y, mActiveCamera->position.z);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetDiffuseLightAccumulationTexture());
	glUniform1i(fpShader->GetUniformLocation(("lightTexDiffuse")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetSpecularLightAccumulationTexture());
	glUniform1i(fpShader->GetUniformLocation(("lightTexSpecular")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetDiffuseTexture());
	glUniform1i(fpShader->GetUniformLocation(("diffuseTex")), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetSpecularTexture());
	glUniform1i(fpShader->GetUniformLocation(("specularTex")), 4);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetDepthTexture());
	glUniform1i(fpShader->GetUniformLocation(("depthTex")), 5);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(fpShader->GetUniformLocation(("charactersTex")), 6);

	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(fpShader->GetUniformLocation("normalTex"), 7);

	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetPositionsTexture());
	glUniform1i(fpShader->GetUniformLocation("posTex"), 8);

	glActiveTexture(GL_TEXTURE0 + 9);

	glUniform1i(fpShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(fpShader->GetUniformLocation(("wHeight")), m_actualHeight);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glViewport(0, 0, m_actualWidth, m_actualHeight);
	//glClearColor(0, 0, 0, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(screenQuad_VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays

														/////////HERE YOU SHOULD DO YOUR STUFF
}

void RenderSystem::BloomPass()
{

	//do BLOOM
	Shader *bloomShader1 = shaders["bloomShader1"];
	Shader *bloomShader2 = shaders["bloomShader2"];
	//pass 1
	bloomShader1->Use();
	bloomFBO1->BindForWriting();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outputFBO->m_texture);// gbuffer.GetFinalTexture());
	glUniform1i(bloomShader1->GetUniformLocation(("frameTex")), 0);

	glUniform1i(bloomShader1->GetUniformLocation(("wWidth")), m_actualWidth / 2);
	glUniform1i(bloomShader1->GetUniformLocation(("wHeight")), m_actualHeight / 2);

	glViewport(0, 0, m_actualWidth / 2, m_actualHeight / 2);

	glBindVertexArray(screenQuad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays
														//pass 2
	bloomShader2->Use();
	bloomFBO2->BindForWriting();
	bloomFBO1->BindForReading(GL_TEXTURE0);
	glUniform1i(bloomShader2->GetUniformLocation(("frameTex")), 0);

	glUniform1i(bloomShader2->GetUniformLocation(("wWidth")), m_actualWidth / 2);
	glUniform1i(bloomShader2->GetUniformLocation(("wHeight")), m_actualHeight / 2);

	glViewport(0, 0, m_actualWidth / 2, m_actualHeight / 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays

}

void RenderSystem::PostProcessingPass()
{

	//////////////////////////////////////////////////////////////////
	Shader *ppShader = shaders["postProcessShader"];
	ppShader->Use();
	
	postProcessFBO->BindForWriting();

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, outputFBO->m_texture);// gbuffer.GetFinalTexture());
	glUniform1i(ppShader->GetUniformLocation(("frameTex")), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormalsTexture());
	glUniform1i(ppShader->GetUniformLocation(("normalTex")), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, outlineFBO1->m_texture);
	glUniform1i(ppShader->GetUniformLocation(("edgeTex")), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, gbuffer.GetMaterialInfoTexture());
	glUniform1i(ppShader->GetUniformLocation(("charactersTex")), 4);

	bloomFBO2->BindForReading(GL_TEXTURE0 + 5);
	glUniform1i(ppShader->GetUniformLocation(("bloomTex")), 5);


	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_2D, ssrFBO->m_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(ppShader->GetUniformLocation(("ssrTex")), 7);


	//directionalShadowMaps[0]->cascadeMaps[0]->BindForReading(GL_TEXTURE0 + 7);
	//glUniform1i(ppShader->GetUniformLocation(("shadowTest")), 7);

	blurredCubeShadowMapFBO->BindForReading(SHADOW_TEXTURE_UNIT(0), ppShader->GetUniformLocation(("cubeShadowMap")));

	glUniform1i(ppShader->GetUniformLocation(("wWidth")), m_actualWidth);
	glUniform1i(ppShader->GetUniformLocation(("wHeight")), m_actualHeight);

	glViewport(0, 0, m_windowWidth, m_windowHeight);

	//glClearColor(0, 0, 0, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays
	glBindVertexArray(0);


	///////////////////////////////////////// FXAA PASS
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	Shader *fxaaShader = shaders["fxaaShader"];
	fxaaShader->Use();

	glUniform1i(fxaaShader->GetUniformLocation(("wWidth")), m_windowWidth);
	glUniform1i(fxaaShader->GetUniformLocation(("wHeight")), m_windowHeight);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, postProcessFBO->m_texture);// gbuffer.GetFinalTexture());
	glUniform1i(fxaaShader->GetUniformLocation(("frameTex")), 1);
	
	glBindVertexArray(screenQuad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays
	glBindVertexArray(0);


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void RenderSystem::RenderMiniMap()
{
	//return;
	InputManager &inputMgr = InputManager::getInstance();

	if (inputMgr.mEditorMode)
		return;

	Shader *depthShader = shaders["miniMapShader"];
	depthShader->Use();
	mMiniMapFBO->BindForWriting();

	glm::mat4 proj = glm::perspective(3.14f * 0.65f, 1.f, 0.1f, 77.f);
	//glm::mat4 proj = glm::ortho(-50.f, 50.f, -50.f, 50.f, 0.01f, 105.f);// glm::perspective(3.14f / 2.f, 1.f, 0.1f, 1000.f);
	CollidableCamera *collidCam = (CollidableCamera *)mActiveCamera;

	glm::vec3 targetPos = collidCam->target_position;
	glm::mat4 view = glm::lookAt(glm::vec3(targetPos.x + 0.001, 75.f, targetPos.z + 0.001), targetPos, collidCam->forward);

	glm::mat4 viewProj = proj * view;
	PerformFrustumCulling(viewProj);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glViewport(0, 0, 512, 512);
	glClearColor(1, 1, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//return;
	for (int i = 0; i < mMeshes.size(); i++)
	{
		glUniformMatrix4fv(depthShader->GetUniformLocation(("P")), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(depthShader->GetUniformLocation(("V")), 1, GL_FALSE, glm::value_ptr(view));
		if (mMeshes[i]->mIsEnabled) {
			glUniformMatrix4fv(depthShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));// mMeshes[i]->mGameObject->mTransform.GetWorldMatrix()));
			mMeshes[i]->RenderFrustumCollisionObjects(m_objectsInFrustumHash,depthShader, 0, false);

		}
	}
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}


void RenderSystem::RenderHUD()
{
	RenderMiniMap();
	Shader *spriteshader = shaders["spriteshader"];
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glm::mat4 mvpMat;
	glm::vec3 camOrient = glm::normalize(glm::vec3(mActiveCamera->forward.x, mActiveCamera->forward.z, 0));
	float angle = -acos(dot(camOrient, glm::vec3(0, 1, 0)));
	glm::vec3 crossProd = glm::vec3(0, 1, 0);

	if (camOrient != glm::vec3(0, -1, 0) && camOrient != glm::vec3(0, 1, 0))
	{
		crossProd = glm::cross(camOrient, glm::vec3(0, 1, 0));

	}
	glm::mat4 screenTransform = glm::translate(glm::mat4(1), glm::vec3(0.62f, -0.66f, 0.f))*
		//mActiveCamera->getProjectionMatrix() *
		glm::perspective(3.14f / 3.f, 1.f, 0.001f, 5.f) *
		glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(1), .84f, glm::normalize(glm::vec3(-1, -0.1, 0)));
	mvpMat = screenTransform *
		glm::rotate(glm::mat4(1), angle, crossProd) *
		glm::scale(glm::mat4(1), glm::vec3(.2));
	glm::mat4 miniMapMVP = screenTransform *glm::scale(glm::mat4(1), glm::vec3(.16));

	mHUD->sprites["miniMap"]->mvpMatrix = mvpMat;
	mHUD->sprites["activeMap"]->mvpMatrix = miniMapMVP;


	mHUD->sprites["activeMap"]->Render(spriteshader);
	for (auto sprite : mHUD->sprites)
	{
		if (!sprite.second->visible) continue;
		if (sprite.first == std::string("activeMap")) continue;
		sprite.second->Render(spriteshader);
	}

	InputManager &inputMgr = InputManager::getInstance();
	if (inputMgr.mEditorMode) {
		ImGui_ImplSdlGL3_NewFrame(inputMgr.mWindow);
		static float f = 0.0f;
		ImGui::Text("Hello, world!");                           // Some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&(glm::vec3(0,0,0))); // Edit 3 floats as a color
		if (ImGui::Button("Demo Window"))                       // Use buttons to toggle our bools. We could use Checkbox() as well.
			printf("sss\n");
		if (ImGui::Button("Another Window"))
			printf("sssssssssssssssss\n");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Render();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::addSpotLight(SpotLight *light) {
	spotLights.push_back(light);
	if (light->mShadowsOn)
	{
		int lID = spotLights.size() - 1;

		staticSpotShadowMapFBOs[lID] = new ShadowMapFBO();
		if (!staticSpotShadowMapFBOs[lID]->Init(AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES))
		{
			printf("Error initializing the shadow map FBO\n");
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::addPointLight(PointLight *light) {
	pointLights.push_back(light);
	if (light->mShadowsOn)
	{
		int lID = pointLights.size() - 1;

		staticCubeShadowMapFBOs[lID] = new ShadowCubeMapFBO();
		if (!staticCubeShadowMapFBOs[lID]->Init(AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES))
		{
			printf("Error initializing the shadow map FBO\n");
		}
	}
}

void RenderSystem::GenerateStaticShadows()
{
	if (generatedShadows)
	{
		return;
	}
	else
	{
		generatedShadows = true;
	}

	Shader *smShader = shaders["shadowMapShader"];
	Shader *hBlurShader = shaders["HshadowBlurShader"];
	Shader *vBlurShader = shaders["VshadowBlurShader"];
	smShader->Use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);


	for (uint lID = 0; lID < pointLights.size(); lID++) {
		if (pointLights[lID]->mShadowsOn)
		{
			for (uint i = 0; i < 6; i++) {
				smShader->Use();
				glEnable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);
				cubeFaceShadowMapFBO->BindForWriting();
				glViewport(0, 0, OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES);
				glClearColor(1.f, 1.f, 1.f, 1.f);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

				glm::mat4 shadow_proj_view = pointLights[lID]->lightCams[i].getProjViewMatrix();

				glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));

				for (uint j = 0; j < mMeshes.size(); j++) {
					if (mMeshes[j]->mShadowCaster)
					{
						glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->mGameObject->mTransform.GetWorldMatrix()));//**TODO MVP *optimization
						mMeshes[j]->Render(smShader, 0, false);
					}
				}
			
				//blur and output to cube map...
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glBindVertexArray(screenQuad_VAO);
				//VBLUR
				vBlurShader->Use();
				cubeFaceShadowMapFBO->BindForReading(GL_TEXTURE0);
				cubeFaceShadowMapFBO2->BindForWriting();
				glUniform1f(vBlurShader->GetUniformLocation("cubeShadowFace"), 0);
				glUniform1i(vBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
				glUniform1i(vBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

				glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				//HBLUR
				hBlurShader->Use();
				cubeFaceShadowMapFBO2->BindForReading(GL_TEXTURE0);
				staticCubeShadowMapFBOs[lID]->BindForWriting(gCubeMapFaces[i]);
				glUniform1f(hBlurShader->GetUniformLocation("cubeShadowFace"), 0);
				glUniform1i(hBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
				glUniform1i(hBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

				glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				glBindVertexArray(0);
				//...
			}
		}
	}

	for (uint lID = 0; lID < spotLights.size(); lID++) {
		if (spotLights[lID]->mShadowsOn)
		{
			smShader->Use();
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			cubeFaceShadowMapFBO->BindForWriting();
			glViewport(0, 0, OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES);
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			glm::mat4 shadow_proj_view = spotLights[lID]->lightCam.getProjViewMatrix();

			glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));

			for (uint j = 0; j < mMeshes.size(); j++) {
				if (mMeshes[j]->mShadowCaster)
				{
					glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->mGameObject->mTransform.GetWorldMatrix()));//**TODO MVP *optimization
					mMeshes[j]->Render(smShader, 0, false);
				}
			}

			//blur and output to cube map...
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glBindVertexArray(screenQuad_VAO);
			//VBLUR
			vBlurShader->Use();
			cubeFaceShadowMapFBO->BindForReading(GL_TEXTURE0);
			cubeFaceShadowMapFBO2->BindForWriting();
			glUniform1f(vBlurShader->GetUniformLocation("cubeShadowFace"), 0);
			glUniform1i(vBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
			glUniform1i(vBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

			glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//HBLUR
			hBlurShader->Use();
			cubeFaceShadowMapFBO2->BindForReading(GL_TEXTURE0);
			staticSpotShadowMapFBOs[lID]->BindForWriting();
			glUniform1f(hBlurShader->GetUniformLocation("cubeShadowFace"), 0);
			glUniform1i(hBlurShader->GetUniformLocation("wWidth"), AA_OMNI_SHADOW_MAP_RES);
			glUniform1i(hBlurShader->GetUniformLocation("wHeight"), AA_OMNI_SHADOW_MAP_RES);

			glViewport(0, 0, AA_OMNI_SHADOW_MAP_RES, AA_OMNI_SHADOW_MAP_RES);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			//...
		}
	}


	////????????????????????????????????//--------------------------------------------------------------savdnvcxzxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////return;
	//Shader *smShader = shaders["shadowMapShader"];
	//smShader->Use();
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glEnable(GL_DEPTH_TEST);
	//glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	//glDepthMask(GL_TRUE);
	//glDisable(GL_BLEND);
	//for (uint lID = 0; lID < pointLights.size(); lID++) {
	//	if (pointLights[lID]->mShadowsOn)
	//	{
	//		for (uint i = 0; i < 6; i++) {

	//			staticCubeShadowMapFBOs[lID]->BindForWriting(gCubeMapFaces[i]);
	//			glViewport(0, 0, OMNI_SHADOW_MAP_RES, OMNI_SHADOW_MAP_RES);
	//			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//			glm::mat4 shadow_proj_view = pointLights[lID]->lightCams[i].getProjViewMatrix();

	//			glUniformMatrix4fv(smShader->GetUniformLocation(("PV")), 1, GL_FALSE, glm::value_ptr(shadow_proj_view));


	//			for (uint j = 0; j < mMeshes.size(); j++) {
	//				if (mMeshes[j]->mShadowCaster)
	//				{
	//					glUniformMatrix4fv(smShader->GetUniformLocation(("Model")), 1, GL_FALSE, glm::value_ptr(mMeshes[j]->GetModelMatrix()));//**TODO MVP *optimization
	//					mMeshes[j]->Render(smShader,0, false);
	//				}
	//			}
	//		}
	//	}
	//}
	//glDepthMask(GL_FALSE);
}

void RenderSystem::addDirectionalLight(DirectionalLight *light)
{
	directionalLights.push_back(light);

	if (light->mShadowsOn)
	{
		int lID = directionalLights.size() - 1;
		directionalShadowMaps[lID] = new CascadedShadowMapFBO();
		if (!directionalShadowMaps[lID]->Init(AA_DIRSHADOWMAP_RES, AA_DIRSHADOWMAP_RES))
		{
			printf("Error initializing the shadow map FBO\n");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


RenderSystem::~RenderSystem()
{
	//TODO
}