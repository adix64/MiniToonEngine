#pragma once
#include <GameObject.h>
#include <SceneGraph.h>
#include <ResourceManager.h>
class ObjectFactory
{
public:
	std::vector<GameObject*> mGameObjects;
	SceneGraph *mSceneRoot;
private:
	ObjectFactory();
	ObjectFactory(ObjectFactory const& copy);            // do NOT implement
	ObjectFactory& operator=(ObjectFactory const& copy); // do NOT implement
public:
	// Stop the compiler from generating copy methods
	static ObjectFactory& getInstance()
	{
		static ObjectFactory instance;
		return instance;
	}

	void Init();
	void ResetGame();
	void CreateLights();
	void FixedUpdate();
	GameObject* CreateCharacter(std::string &fileName, bool controlled = false);
	void AddObject(GameObjCreationSettings &cs);
};