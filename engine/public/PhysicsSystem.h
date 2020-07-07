#pragma once
#include <unordered_map>
#include "../../../dependencies/bullet3-2.86.1/src/btBulletCollisionCommon.h"
#include "../../../dependencies/bullet3-2.86.1/src/btBulletDynamicsCommon.h"

class Character;

class PhysicsEngine
{


private:
	PhysicsEngine();
	// Stop the compiler from generating copy methods
	PhysicsEngine(PhysicsEngine const& copy);            // do NOT implement
	PhysicsEngine& operator=(PhysicsEngine const& copy); // do NOT implement
public:
	void Init();
	static PhysicsEngine& getInstance()
	{
		static PhysicsEngine instance;
		return instance;
	}

	btDefaultCollisionConfiguration* collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	~PhysicsEngine();
#define BIT(x) (1<<(x))
	enum collisiontypes {
		COL_NOTHING = 0,
		COL_TORI_BODY = BIT(0),
		COL_UKE_BODY = BIT(1),
		COL_TORI_LIMB = BIT(2),
		COL_UKE_LIMB = BIT(3),
		COL_WORLD = BIT(4),
		COL_RAY = BIT(5)
	};
};