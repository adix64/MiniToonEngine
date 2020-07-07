#include <SceneGraph.h>
#include <queue>
#include "../../dependencies/glm/gtx/quaternion.hpp"
#include "../../dependencies/glm/gtx/transform.hpp"
#include <Component/MeshRenderer.h>
#include <Component/SkinnedMeshRenderer.h>
#include <RenderingSystem.h>
void SceneGraph::_UpdateObjectTransformHierarchy(GameObject *node, glm::mat4 &pParentWorldTransfrom)
{
	if (node->mTransform.mUpdated)
	{
		for (int i = 0, sz = node->mChildren.size(); i < sz; i++)
			_UpdateObjectTransformHierarchy(node->mChildren[i], node->mTransform.mWorldMatrix);
		return;
	}
	node->mTransform.mUpdated = true;

	node->mTransform.mLocalMatrix = glm::translate(glm::mat4(1), node->mTransform.mPosition)
								  * glm::toMat4(node->mTransform.mRotation)
								  * glm::scale(glm::mat4(1), node->mTransform.mScale);
	const glm::mat4 worldMat = pParentWorldTransfrom * node->mTransform.mLocalMatrix;
	node->mTransform.mWorldMatrix = worldMat;

	node->mTransform.mDirection = glm::normalize(glm::vec3(worldMat * glm::vec4(0, 0, 1, 0)));
	node->mTransform.mRight = glm::normalize(glm::vec3(worldMat * glm::vec4(1, 0, 0, 0)));
	node->mTransform.mUp = glm::normalize(glm::cross(node->mTransform.mRight, node->mTransform.mDirection));

	SkinnedMeshRenderer *skMesh = node->GetComponentByType<SkinnedMeshRenderer>();
	if (skMesh)
	{
		RenderSystem &renderSys = RenderSystem::getInstance();
		
		btTransform trans;
		trans.setOrigin(btVector3(node->mTransform.mPosition.x, node->mTransform.mPosition.y, node->mTransform.mPosition.z));
		trans.setRotation(btQuaternion(node->mTransform.mRotation.x, node->mTransform.mRotation.y, node->mTransform.mRotation.z, node->mTransform.mRotation.w));
		
		((btRigidBody*)(skMesh->mFrustumCollisionObject))->getMotionState()->setWorldTransform(trans);
		((btRigidBody*)(skMesh->mFrustumCollisionObject))->setActivationState(ACTIVE_TAG);
		//renderSys.m_collisionWorld->removeRigidBody((btRigidBody*)(skMesh->mFrustumCollisionObject));
		//renderSys.m_collisionWorld->addRigidBody((btRigidBody*)(skMesh->mFrustumCollisionObject));
	}

	MeshRenderer *mesh = node->GetComponentByType<MeshRenderer>();
	if (mesh && !mesh->mIsSkybox && mesh->mRenderable)
	{
		btVector3 pos = btVector3(node->mTransform.mPosition.x, node->mTransform.mPosition.y, node->mTransform.mPosition.z);
		btQuaternion rot = btQuaternion(node->mTransform.mRotation.x, node->mTransform.mRotation.y, node->mTransform.mRotation.z, node->mTransform.mRotation.w);
		btTransform trans;// = ((btRigidBody*)(colobj.second))->getWorldTransform();
		trans.setOrigin(pos);
		trans.setRotation(rot);

		mesh->SetDbvtWorldTransform(trans);
	}


	for (int i = 0, sz = node->mChildren.size(); i < sz; i++)
		_UpdateObjectTransformHierarchy(node->mChildren[i], node->mTransform.mWorldMatrix);

}

void SceneGraph::UpdateObjectTransformHierarchy()
{
	_UpdateObjectTransformHierarchy(this, glm::mat4(1));
}
