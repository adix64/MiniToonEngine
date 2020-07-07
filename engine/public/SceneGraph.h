#pragma once
#include <GameObject.h>


class SceneGraph : public GameObject
{
public:
	SceneGraph(std::string &name) : GameObject(name) {}
	void UpdateObjectTransformHierarchy();
	void _UpdateObjectTransformHierarchy(GameObject *node, glm::mat4 &mParentWorldTransfrom);

};