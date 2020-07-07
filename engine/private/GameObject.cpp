#include <GameObject.h>
#include <queue>
#include <Component/Component.h>
#include <Component/Transform.h>

GameObject::GameObject()
{
	//this->mTransform = new Transform();
	this->mParent = NULL;
	this->mName = "GameObject(0)";
}

GameObject::GameObject(std::string &pName)
{
	//this->mTransform = new Transform();
	this->mParent = NULL;
	this->mName = pName;
}

GameObject::~GameObject()
{
	//TODO
}

void GameObject::AddComponent(Component *pComp)
{
	pComp->mGameObject = this;
	mComponents.push_back(pComp);
}

void GameObject::AddChild(GameObject *pChild)
{
	pChild->mParent = this;
	mChildren.push_back(pChild);
}

void GameObject::RemoveChild(GameObject *pChild)
{
	//TODO
	//std::
	//mChildren.erase(pChild);
}


Component* GameObject::GetComponentByName(std::string &pCompName)
{
	for (unsigned i = 0, sz = mComponents.size(); i < sz; i++)
	{
		Component *comp = mComponents[i];
		if (comp->mName == pCompName)
			return comp;
	}
	return NULL;
}

GameObject* GameObject::GetChildByName(std::string &pChildName)
{
	std::queue<GameObject*> que;
	for (unsigned i = 0, sz = mChildren.size(); i < sz; i++)
	{
		que.push(mChildren[i]);
	}
	while (!que.empty())
	{
		GameObject *gameObj = que.front();
		que.pop();
		if (gameObj->mName == pChildName)
			return gameObj;
		for (unsigned i = 0, sz = gameObj->mChildren.size(); i < sz; i++)
		{
			que.push(gameObj->mChildren[i]);
		}
	}
	return NULL;
}

std::vector<GameObject*> GameObject::GetAllChildrenByName(std::string &pChildName)
{
	std::queue<GameObject*> que;
	std::vector<GameObject*> ret;
	for (unsigned i = 0, sz = mChildren.size(); i < sz; i++)
	{
		que.push(mChildren[i]);
	}
	while (!que.empty())
	{
		GameObject *gameObj = que.front();
		que.pop();
		if (gameObj->mName == pChildName)
			ret.push_back(gameObj);
		for (unsigned i = 0, sz = gameObj->mChildren.size(); i < sz; i++)
		{
			que.push(gameObj->mChildren[i]);
		}
	}
	return ret;
}