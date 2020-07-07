#pragma once
#include <Component/Component.h>
#include <PhysicsSystem.h>
class Collider : public Component
{
public:
	Collider()
	{
		mRigidBodies.resize(1, NULL);
		mCollisionShapes.resize(1, NULL);
	}

	std::vector<btRigidBody*> mRigidBodies;
	std::vector<btCollisionShape*> mCollisionShapes;
	void Destroy();
	void InitFromFile(std::string &fileName);
};