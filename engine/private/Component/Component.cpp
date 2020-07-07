#include <Component/Component.h>

Component::Component()
{
	this->mGameObject = NULL;
	this->mIsEnabled = true;
	this->mName = "Component";
}
Component::Component(std::string &pName)
{
	this->mGameObject = NULL;
	this->mIsEnabled = true;
	this->mName = pName;
}