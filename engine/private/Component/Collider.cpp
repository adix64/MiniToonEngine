#include <Component/Collider.h>
#include <ResourceManager.h>
#include "../../dependencies/glm/glm.hpp"
void Collider::Destroy()
{
	for (int i = 0; i < mRigidBodies.size(); i++)
	{
		if (!mRigidBodies[i])
			continue;
		PhysicsEngine::getInstance().dynamicsWorld->removeRigidBody(mRigidBodies[i]);
		delete mRigidBodies[i];
	}
	for (int i = 0; i < mCollisionShapes.size(); i++)
	{
		if (!mCollisionShapes[i])
			continue;
		btCollisionShape *colShape = mCollisionShapes[i];
		PhysicsEngine::getInstance().collisionShapes.remove(mCollisionShapes[i]);
		delete colShape;
	}
	mRigidBodies.resize(0);
	mCollisionShapes.resize(0);
}

bool vtxinvec(glm::vec3 &vtx, std::vector<glm::vec3> &vec)
{

	for (int i = 0; i < vec.size(); i++)
	{
		if (glm::distance(vtx, vec[i]) < 0.001f)
			return true;
	}
	return false;
}

void Collider::InitFromFile(std::string &fileName)
{
	Destroy();
	ResourceManager &resMgr = ResourceManager::getInstance();
	const aiScene* scene = resMgr.mImporter.ReadFile("../assets/staticColliders.dae", aiProcess_OptimizeMeshes);

	if (!scene)
	{
		printf("ERROR: Could not load static collision objects.\n");
		return;
	}
	//glm::mat4 shrink = glm::scale(glm::mat4(1), glm::vec3(0.032, 0.032, 0.032));
	for (int i = 0; i < scene->mNumMeshes; i++)
	{


		const aiMesh* mesh = scene->mMeshes[i];
		std::vector<glm::vec3> verts;


		std::vector<btScalar> points;
		for (int v = 0; v < mesh->mNumVertices; v++) {
			glm::vec4 vert = glm::vec4(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z, 1);
			//glm::vec3 vtx = glm::vec3(shrink * vert);
			glm::vec3 vtx = glm::vec3(vert);

			if (!vtxinvec(vtx, verts))
			{
				verts.push_back(vtx);
				points.push_back(vtx.x);
				points.push_back(vtx.y);
				points.push_back(vtx.z);
			}
		}
		btCollisionShape* groundShape = new btConvexHullShape((const btScalar *)(&points[0]), verts.size(), 3 * sizeof(btScalar));
		mCollisionShapes.push_back(groundShape);
		PhysicsEngine::getInstance().collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 0, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setFriction(0);
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

		//add the body to the dynamics world
		int group = PhysicsEngine::collisiontypes::COL_WORLD;
		int mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_WORLD
			| PhysicsEngine::collisiontypes::COL_RAY;

		PhysicsEngine::getInstance().dynamicsWorld->addRigidBody(body, group, mask);
		mRigidBodies.push_back(body);
	}
	//delete scene;
}