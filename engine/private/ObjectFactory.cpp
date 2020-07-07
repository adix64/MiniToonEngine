#include <ObjectFactory.h>
#include <ResourceManager.h>
#include <Component/MeshRenderer.h>
#include <Component/SkinnedMeshRenderer.h>
#include <Component/Collider.h>
#include <Component/CharacterAnimation/CharacterAnimator.h>
#include <GameObjects/Character.h>
#include <RenderingSystem.h>
//#include <CharacterAnimation\>
//#define DEBUG_CHARACTER_BOUNDING_CAPSULE
#define SHOW_ENEMY
//#define SHOW_ENEMY2

ObjectFactory::ObjectFactory() {}


void ObjectFactory::ResetGame()
{
	mGameObjects[0]->GetComponentByType<Character>()->SetPosition(glm::vec3(200, 7, 0), glm::vec3(-1, 0.25, 0));
	Character *chr = mGameObjects[1]->GetComponentByType<Character>();
	if (chr)
		chr->SetPosition(glm::vec3(5, 6, -30), glm::vec3(-1, 0.25, 0));

	chr = mGameObjects[2]->GetComponentByType<Character>();
	if (chr)
		chr->SetPosition(glm::vec3(5, 6, 10), glm::vec3(-1, 0.25, 0));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFactory::CreateLights()
{
	RenderSystem &renderingEngine = RenderSystem::getInstance();
	///
	//{
	//	DirectionalLight *light = new DirectionalLight(0.7, 0.01, glm::normalize(glm::vec3(1, 1, 0.4)), glm::vec3(0.95, 0.9, 1));
	////	////light->mShadowsOn = false;
	//	renderingEngine.addDirectionalLight(light);
	//}

	//SpotLight *spot_light2 = new SpotLight(1, 0.2, glm::vec3(3, 25, 0), glm::vec3(0.1, -1, 0), 0.7, 0.8, glm::vec3(0, 1, 0), glm::vec3(1, 0.0002, 0.0005));
	////spot_light2->mShadowsOn = false;
	//renderingEngine.addSpotLight(spot_light2);

	//SpotLight *spot_light3 = new SpotLight(1, 0.2, glm::vec3(100, 15, -15), glm::vec3(1, -1, 0), 0.7, 0.8, glm::vec3(1, 1, 0), glm::vec3(1, 0.0002, 0.0005));
	////spot_light3->mShadowsOn = false;

	//renderingEngine.addSpotLight(spot_light3);


	//

	//PointLight *point_light1 = new PointLight(1.25, 0.1, glm::vec3(0, 20, 0), glm::vec3(1, 0.75, 0.5), glm::vec3(1, 0.0002, 0.0005), true);
	//renderingEngine.addPointLight(point_light1);

	//PointLight *point_light2 = new PointLight(1.5, 0.1, glm::vec3(100, 20, -15), glm::vec3(0.5, 0.75, 1), glm::vec3(1, 0.0002, 0.0005), true);
	//renderingEngine.addPointLight(point_light2);
	
	//return;


	ResourceManager &resMgr = ResourceManager::getInstance();
	const aiScene* lightScene = resMgr.mImporter.ReadFile("../assets/lightsFAST.dae", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	for (int i = 0; i < lightScene->mNumLights; i++)
	{
		PointLight *pl;
		SpotLight *sl;
		DirectionalLight *dl;
		aiLight *l = lightScene->mLights[i];
		glm::vec3 lPos, lDir;
		aiNode *lNode;
		
		glm::mat4 lightWorldMatrix;
		switch (l->mType)
		{
		case aiLightSource_DIRECTIONAL:
			lNode = lightScene->mRootNode->FindNode(l->mName);
				lDir = -glm::vec3(convertMatrix4x4(lightScene->mRootNode->FindNode(l->mName)->mTransformation) * glm::vec4(l->mDirection.x, l->mDirection.y, l->mDirection.z, 0));
				lDir = glm::normalize(lDir);
				dl = new DirectionalLight(1.f, l->mColorAmbient.r, lDir, glm::vec3(l->mColorDiffuse.r, l->mColorDiffuse.g, l->mColorDiffuse.b), strstr(l->mName.data, "SDW") != NULL);
				renderingEngine.addDirectionalLight(dl);
			break;
		case aiLightSource_SPOT:
			lNode = lightScene->mRootNode->FindNode(l->mName);
			lightWorldMatrix = convertMatrix4x4(lNode->mTransformation);
			lPos = glm::vec3(lightWorldMatrix * glm::vec4(l->mPosition.x, l->mPosition.y, l->mPosition.z, 1));
			lDir = glm::vec3(lightWorldMatrix * glm::vec4(l->mDirection.x, l->mDirection.y, l->mDirection.z, 0));
			lDir = glm::normalize(lDir);
			sl = new SpotLight(1, 0.2, lPos,
				glm::normalize(lDir),
				cos(l->mAngleInnerCone * .5f * (3.f - 1.618f)), cos(l->mAngleInnerCone * .5f),
				glm::vec3(l->mColorDiffuse.r, l->mColorDiffuse.g, l->mColorDiffuse.b),
				glm::vec3(l->mAttenuationConstant, l->mAttenuationLinear, l->mAttenuationQuadratic));
			sl->mShadowsOn =  strstr(l->mName.data, "SDW") != NULL;
			renderingEngine.addSpotLight(sl);

			break;
		case aiLightSource_POINT:
			/*lNode = lightScene->mRootNode->FindNode(l->mName);
			lPos = glm::vec3(convertMatrix4x4(lNode->mTransformation) * glm::vec4(l->mPosition.x, l->mPosition.y, l->mPosition.z, 1));
			pl = new PointLight(1.f, 0.2, lPos,
				glm::vec3(l->mColorDiffuse.r, l->mColorDiffuse.g, l->mColorDiffuse.b),
				glm::vec3(l->mAttenuationConstant, l->mAttenuationLinear, l->mAttenuationQuadratic), strstr(l->mName.data, "SDW") != NULL);
			renderingEngine.addPointLight(pl);*/
			break;
		}

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFactory::Init()
{
	this->mSceneRoot = new SceneGraph(std::string("SceneRoot"));
	ResourceManager &resMgr = ResourceManager::getInstance();

	GameObject *tori = CreateCharacter(std::string("Tori"), true);
#ifdef SHOW_ENEMY
	{
		GameObject *uke = CreateCharacter(std::string("Uke"), false);
		Character *toriCtrl = tori->GetComponentByType<Character>();
		Character *ukeCtrl = uke->GetComponentByType<Character>();
		toriCtrl->AddEnemy(ukeCtrl);
		ukeCtrl->AddEnemy(toriCtrl);
	}
#endif
#ifdef SHOW_ENEMY2
	{
		GameObject* uke = CreateCharacter(std::string("Uke"), false);
		Character* toriCtrl = tori->GetComponentByType<Character>();
		Character* ukeCtrl = uke->GetComponentByType<Character>();
		toriCtrl->AddEnemy(ukeCtrl);
		ukeCtrl->AddEnemy(toriCtrl);
	}
#endif


	auto environment = resMgr.GetEnvironment();
	for (int i = 0; i < environment.size(); i++)
	{
		AddObject(environment[i]);
	}

	CreateLights();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GameObject* ObjectFactory::CreateCharacter(std::string &chName, bool controlled)
{
	ResourceManager &resMgr = ResourceManager::getInstance();
	GameObject *gameObject = new GameObject(chName);
	

	SkinnedMeshRenderer *skinnedMesh = new SkinnedMeshRenderer(resMgr.GetCharacterFileName(chName.c_str()));
	gameObject->AddComponent(skinnedMesh);
#ifdef DEBUG_CHARACTER_BOUNDING_CAPSULE
	MeshRenderer *mesh = new MeshRenderer(resMgr.GetCharacterColShapeFname());
	mesh->mWireframeDisplay = true;
	gameObject->AddComponent(mesh);
	mesh->Awake();
#endif
	Collider *collider = new Collider();
	gameObject->AddComponent(collider);

	Character *characterScript = NULL;
	if (controlled) {
		characterScript = new ControllableCharacter(chName.c_str(), false);
	}
	else
	{
		characterScript = new AICharacter(chName.c_str(), true);
	}
	gameObject->AddComponent(characterScript);

	CharacterAnimator *characterAnimator = new CharacterAnimator();// chName, skinnedMesh, &(gameObject->mTransform), &(characterScript->m_moveDirection), &(characterScript->m_enemyDirection));
	gameObject->AddComponent(characterAnimator);

	mSceneRoot->AddChild(gameObject);

	skinnedMesh->Awake();
	collider->Awake();
	characterScript->Awake();
	characterAnimator->Awake();
	mGameObjects.push_back(gameObject);
	return gameObject;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFactory::AddObject(GameObjCreationSettings &cs)
{
	GameObject *gameObject = new GameObject(cs.name);
	
	if (cs.fileName != std::string(""))
	{// add Mesh Renderer Component
		MeshRenderer *meshRenderer = new MeshRenderer(cs.fileName);
		gameObject->AddComponent(meshRenderer);
		if (cs.isSkybox)
		{
			meshRenderer->mIsSkybox = true;
			meshRenderer->depthOverride = cs.depthOverride;
		}
		meshRenderer->Awake();
	}
	if (cs.colliderFileName != std::string(""))
	{// add Collider Component
		Collider *meshCollider = new Collider();
		meshCollider->InitFromFile(cs.colliderFileName);
		gameObject->AddComponent(meshCollider);
		meshCollider->Awake();
	}
	//Place in Scene Hierarchy
	mSceneRoot->AddChild(gameObject);
	mGameObjects.push_back(gameObject);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFactory::FixedUpdate()
{
	mSceneRoot->UpdateObjectTransformHierarchy();
}