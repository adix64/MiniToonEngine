#pragma once
#include <string>
#include <vector>
#include <Component/Transform.h>
class Component; 

class GameObject
{
public:
	std::string mName;
	GameObject *mParent;
	std::vector<GameObject*> mChildren;

	Transform mTransform;
	std::vector<Component*> mComponents;

public:
	GameObject();
	GameObject(std::string &pName);
	~GameObject();

	void AddComponent(Component *pComp);
	void AddChild(GameObject *pChild);
	void RemoveChild(GameObject *pChild);

	Component* GetComponentByName(std::string &pCompName);
	template<typename T> T* GetComponentByType()
	{
		for (std::vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
			if (T* comp = dynamic_cast<T*>(*it))
				return comp;
		return NULL;
	};

	GameObject* GetChildByName(std::string &pChildName);
	std::vector<GameObject*> GetAllChildrenByName(std::string &pChildName);
};