#pragma once
#include <GameObjects/Character.h>
#include <RenderingSystem.h>
#include <Component/CharacterAnimation/ChAnimDefines.h>
#include <Component/CharacterAnimation/CharacterAnimator.h>
#include <GameObject.h>
#include "../../dependencies/glm/gtx/euler_angles.hpp"
#include <ResourceManager.h>
#include <ObjectFactory.h>
#include <Component/Collider.h>
#include <GameLogicSystem.h>
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#define ENEMY_ENGAGE_RANGE 12.5f

ResourceManager& resMgr = ResourceManager::getInstance();

Character::Character(const char *pName, bool isUke)
{
	//_CrtDumpMemoryLeaks();

	mIsUke = isUke;
	mName = std::string(pName);
	oldTargetPos = glm::vec3(0, 0, 0);
	GameLogicSystem &gameLogicSys = GameLogicSystem::getInstance();
	gameLogicSys.mScripts.push_back(this);
	RenderSystem &renderSys = RenderSystem::getInstance();
	renderSys.SetBoomRay(mName, false);
}

void Character::Awake()
{
	mTransform = &(mGameObject->mTransform);
	mSkinnedMesh = mGameObject->GetComponentByType<SkinnedMeshRenderer>();
	m_Animator = mGameObject->GetComponentByType<CharacterAnimator>();
	mCollider = mGameObject->GetComponentByType<Collider>();

	enemyEngageTime = 0.f;
	InitWorldTransforms();

	InputManager &inputManager = InputManager::getInstance();

}

////////////////////////////////////////////////////////////////////////////////

void Character::AddEnemy(Character *enemy)
{
	enemies.push_back(enemy);
}

////////////////////////////////////////////////////////////////////////////////

Character* Character::closestEnemy()
{
	if (!enemies.empty()) {
		float minDist = glm::distance(mTransform->GetPosition(), enemies[0]->mTransform->GetPosition());
		float dist;
		Character *closest_enemy = enemies[0];
		for (int i = 1; i < enemies.size(); i++) {
			dist = glm::distance(mTransform->GetPosition(), enemies[i]->mTransform->GetPosition());
			if (dist < minDist) {
				minDist = dist;
				//return closest_enemy;
				closest_enemy = enemies[i];
			}
		}
		return closest_enemy;
	}
	else 
		return NULL;
}

////////////////////////////////////////////////////////////////////////////////

SkinnedMeshRenderer* Character::getSkinnedMesh()
{
	return mSkinnedMesh;
}

////////////////////////////////////////////////////////////////////////////////

bool isMatValid(glm::mat4 &mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; i < 4; i++)
		{
			if (isnan(mat[i][j]))
				return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void Character::InitWorldTransforms()
{
	PhysicsEngine &physicsEngine = PhysicsEngine::getInstance();

	if (mCollider->mCollisionShapes[0]) {
		delete mCollider->mRigidBodies[0];
		mCollider->mRigidBodies[0] = NULL;
	}

	mCollider->mCollisionShapes[0] = new btCapsuleShape(2.1f, 5.f);
	//mColShapeLAnkle = new btBoxShape(btVector3(0.2, 0.2, 0.2));
	//mColShapeRAnkle = new btBoxShape(btVector3(0.2, 0.2, 0.2));
	//mColShapeLHand = new btBoxShape(btVector3(0.2, 0.2, 0.2));
	//mColShapeRHand = new btBoxShape(btVector3(0.2, 0.2, 0.2));

	//mColShapeLAnkle = new btSphereShape(0.1f);
	//mColShapeRAnkle = new btSphereShape(0.1f);
	//mColShapeLHand = new btSphereShape(0.1f);
	//mColShapeRHand = new btSphereShape(0.1f);


	physicsEngine.collisionShapes.push_back(mCollider->mCollisionShapes[0]);

	if (mCollider->mRigidBodies[0]) {
		delete mCollider->mRigidBodies[0];
		mCollider->mRigidBodies[0] = NULL;
	}
	//physicsEngine.collisionShapes.push_back(mColShapeLAnkle);
	//physicsEngine.collisionShapes.push_back(mColShapeRAnkle);
	//physicsEngine.collisionShapes.push_back(mColShapeLHand);
	//physicsEngine.collisionShapes.push_back(mColShapeRHand);

	ResetCapsuleRigidBody(mCollider->mRigidBodies[0],mCollider->mCollisionShapes[0], glm::vec3(20, 6, 30), 9000);
	
	//float ukeOffset = mIsUke ? 50 : 100;
	//ResetKinematicRigidBody(mRigidBodyLAnkle, mColShapeLAnkle, mMotionStateLAnkle, glm::vec3(25, 30 + mIsUke, 40));
	//ResetKinematicRigidBody(mRigidBodyRAnkle, mColShapeRAnkle, mMotionStateRAnkle, glm::vec3(30, 30 + mIsUke, 35));
	//ResetKinematicRigidBody(mRigidBodyLHand, mColShapeLHand, mMotionStateLHand, glm::vec3(35, 30 + mIsUke, 30));
	//ResetKinematicRigidBody(mRigidBodyRHand, mColShapeRHand, mMotionStateRHand, glm::vec3(40, 30 + mIsUke, 25));

	/*int group, mask;
	if (mIsUke)
	{
		group = PhysicsEngine::collisiontypes::COL_UKE_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			 | PhysicsEngine::collisiontypes::COL_TORI_BODY
			 | PhysicsEngine::collisiontypes::COL_TORI_LIMB
			 | PhysicsEngine::collisiontypes::COL_WORLD;
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBody, group, mask);

		group = PhysicsEngine::collisiontypes::COL_UKE_LIMB;
		mask = PhysicsEngine::collisiontypes::COL_TORI_BODY;
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyRAnkle, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyLAnkle, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyRHand, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyLHand, group, mask);
	}
	else
	{
		group = PhysicsEngine::collisiontypes::COL_TORI_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_UKE_LIMB
			| PhysicsEngine::collisiontypes::COL_WORLD;
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBody, group, mask);

		group = PhysicsEngine::collisiontypes::COL_TORI_LIMB;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY;
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyRAnkle, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyLAnkle, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyRHand, group, mask);
		physicsEngine.dynamicsWorld->addRigidBody(mRigidBodyLHand, group, mask);
	}*/
	

	/*
	btTriangleMesh::btTriangleMesh(bool use32bitIndices, bool use4componentVertices)
	void    btTriangleMesh::addIndex(int index)
	void    btTriangleMesh::addTriangleIndices(int index1, int index2, int index3)
	int     btTriangleMesh::findOrAddVertex(const btVector3& vertex, bool removeDuplicateVertices)
	void    btTriangleMesh::addTriangle(const btVector3& vertex0, const btVector3& vertex1, const btVector3& vertex2, bool removeDuplicateVertices)
	int btTriangleMesh::getNumTriangles() const
	void btTriangleMesh::preallocateVertices(int numverts)
	void btTriangleMesh::preallocateIndices(int numindices)
	*/
}

void Character::SetPosition(glm::vec3 &pos, glm::vec3 &lookDir)
{
	camera.set(pos, lookDir, glm::vec3(0, 1, 0));
	Collider *collider = mGameObject->GetComponentByType<Collider>();
	ResetCapsuleRigidBody(mCollider->mRigidBodies[0],mCollider->mCollisionShapes[0], pos, 9000);

	m_Animator->m_currentState = &(m_Animator->m_defaultGroundedState);
	m_Animator->m_crtTime = 0.f;
	HP = 100.f;
	mDeadCounter = 0;
	mAlive = true;
	return;
	if (glm::distance(lookDir, glm::vec3(0, 0, 1)) > 0.0001)
	{
		btTransform transf = mCollider->mRigidBodies[0]->getWorldTransform();
		btQuaternion q;
		glm::vec3 lookDirOnGround = glm::normalize(glm::vec3(lookDir.x, 0, lookDir.z));
		float dirfact = glm::normalize(glm::cross(glm::vec3(0, 0, 1), lookDirOnGround)).y;
		q.setEuler(dirfact * glm::acos(glm::clamp(glm::dot(glm::vec3(0, 0, 1), lookDirOnGround), -1.f, 1.f)), 0, 0);
		transf.setRotation(q);
		mCollider->mRigidBodies[0]->setWorldTransform(transf);
	}
	int group, mask;
	if (mIsUke)
	{
		group = PhysicsEngine::collisiontypes::COL_UKE_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_LIMB
			| PhysicsEngine::collisiontypes::COL_WORLD;
		//PhysicsEngine::getInstance().dynamicsWorld->addRigidBody(mCollider->mRigidBodies[0], group, mask);
	}
	else
	{
		group = PhysicsEngine::collisiontypes::COL_TORI_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_UKE_LIMB
			| PhysicsEngine::collisiontypes::COL_WORLD;
		//PhysicsEngine::getInstance().dynamicsWorld->addRigidBody(mCollider->mRigidBodies[0], group, mask);
	}
	
}

////////////////////////////////////////////////////////////////////////////////

void Character::ResetKinematicRigidBody(btRigidBody *&rigidBody, btCollisionShape *&colShape, MyKinematicMotionState *&motionState, glm::vec3 &newPos)
{
	if (rigidBody)
	{
		PhysicsEngine::getInstance().dynamicsWorld->removeRigidBody(rigidBody);
		delete rigidBody;
	}
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	
	startTransform.setOrigin(btVector3(newPos.x, newPos.y, newPos.z));

	motionState = new MyKinematicMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, motionState, colShape, localInertia);
	rigidBody = new btRigidBody(rbInfo);
	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
}


////////////////////////////////////////////////////////////////////////////////

void Character::ResetCapsuleRigidBody(btRigidBody *&rigidBody, btCollisionShape *&colShape, glm::vec3 &newPos, float _mass)
{
	//return;
	if (rigidBody)
	{
		PhysicsEngine::getInstance().dynamicsWorld->removeRigidBody(rigidBody);
		delete rigidBody;
	}
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar	mass(_mass);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(newPos.x, newPos.y, newPos.z));

	btDefaultMotionState *mMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
	rigidBody = new btRigidBody(rbInfo);	
	rigidBody->setAngularFactor(btVector3(0, 1, 0));
	//btRigidBody *rigidBody = mGameObject->GetComponentByType<Collider>()->mRigidBodies[0];
	rigidBody->setFriction(0);
	PhysicsEngine &physicsEngine = PhysicsEngine::getInstance();

	int group, mask;
	if (mIsUke)
	{
		group = PhysicsEngine::collisiontypes::COL_UKE_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_LIMB
			| PhysicsEngine::collisiontypes::COL_WORLD;
	}
	else
	{
		group = PhysicsEngine::collisiontypes::COL_TORI_BODY;
		mask = PhysicsEngine::collisiontypes::COL_UKE_BODY
			| PhysicsEngine::collisiontypes::COL_TORI_BODY
			| PhysicsEngine::collisiontypes::COL_UKE_LIMB
			| PhysicsEngine::collisiontypes::COL_WORLD;
	}

	PhysicsEngine::getInstance().dynamicsWorld->addRigidBody(rigidBody, group, mask);
}
/////////////////////////////////////////////////////////////////////////////////////////////////



void Character::SetLimbsPositions(glm::mat4 &trans, glm::mat4 &rot)
{
	return;
	//glm::mat4 crtObjSpaceBoneTransform;
	//glm::mat4 bodyMat = mSkinnedMesh->modelMatrix;
	//glm::mat4 strangeScale = glm::scale(glm::vec3(30, 30, 30));
	//glm::mat4 deflateScale = glm::scale(glm::vec3(0.0328, 0.0328, 0.0328));
	//int limbID;

	//limbID = mSkinnedMesh->rightAnkleID;
	//crtObjSpaceBoneTransform = mSkinnedMesh->m_prevRightAnkleTransform;
	//ch_rightAnkle->modelMatrix = mSkinnedMesh->modelMatrix * crtObjSpaceBoneTransform
	//										* glm::inverse(mSkinnedMesh->m_BoneInfo[limbID].BoneOffset)*  strangeScale;

	//limbID = mSkinnedMesh->leftAnkleID;
	//crtObjSpaceBoneTransform = mSkinnedMesh->m_prevLeftAnkleTransform;
	//ch_leftAnkle->modelMatrix = mSkinnedMesh->modelMatrix * crtObjSpaceBoneTransform
	//	* glm::inverse(mSkinnedMesh->m_BoneInfo[limbID].BoneOffset)*  strangeScale;

	//limbID = mSkinnedMesh->rightHandID;
	//crtObjSpaceBoneTransform = mSkinnedMesh->m_prevRightHandTransform;
	//ch_rightWrist->modelMatrix = mSkinnedMesh->modelMatrix * crtObjSpaceBoneTransform
	//	* glm::inverse(mSkinnedMesh->m_BoneInfo[limbID].BoneOffset)*  strangeScale;

	//limbID = mSkinnedMesh->leftHandID;
	//crtObjSpaceBoneTransform = mSkinnedMesh->m_prevLeftHandTransform;

	//ch_leftWrist->modelMatrix = mSkinnedMesh->modelMatrix * crtObjSpaceBoneTransform
	//	* glm::inverse(mSkinnedMesh->m_BoneInfo[limbID].BoneOffset)*  strangeScale;

	//glm::vec3 limbPos;
	//btTransform realTransform;
	//btTransform transform;
	//transform.setRotation(btQuaternion(0, 1, 0, 0));
	//btVector3 dS, realPos, btLimbPos;

	//limbPos = glm::vec3(ch_leftAnkle->modelMatrix * glm::vec4(0, 0, 0, 1));
	//btLimbPos = btVector3(limbPos.x, limbPos.y, limbPos.z);
	//transform.setOrigin(btLimbPos);
	//mRigidBodyLAnkle->setWorldTransform(transform);
	//mMotionStateLAnkle->setKinematicPos(transform);

	//limbPos = glm::vec3(ch_rightAnkle->modelMatrix * glm::vec4(0, 0, 0, 1));
	//btLimbPos = btVector3(limbPos.x, limbPos.y, limbPos.z);
	//transform.setOrigin(btLimbPos);
	//mRigidBodyRAnkle->setWorldTransform(transform);
	//mMotionStateRAnkle->setKinematicPos(transform);

	//limbPos = glm::vec3(ch_leftWrist->modelMatrix * glm::vec4(0, 0, 0, 1));
	//btLimbPos = btVector3(limbPos.x, limbPos.y, limbPos.z);
	//transform.setOrigin(btLimbPos);
	//mRigidBodyLHand->setWorldTransform(transform);
	//mMotionStateLHand->setKinematicPos(transform);

	//limbPos = glm::vec3(ch_rightWrist->modelMatrix * glm::vec4(0, 0, 0, 1));
	//btLimbPos = btVector3(limbPos.x, limbPos.y, limbPos.z);
	//transform.setOrigin(btLimbPos);
	//mRigidBodyRHand->setWorldTransform(transform);
	//mMotionStateRHand->setKinematicPos(transform);
	//
	//
	//btVector3 btPos;
	//transform = mRigidBodyLAnkle->getWorldTransform();
	//btPos = transform.getOrigin();
	//ch_leftAnkle->modelMatrix = glm::translate(glm::mat4(1), glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ()));

	//transform = mRigidBodyRAnkle->getWorldTransform();
	//btPos = transform.getOrigin();
	//ch_rightAnkle->modelMatrix = glm::translate(glm::mat4(1), glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ()));

	//transform = mRigidBodyLHand->getWorldTransform();
	//btPos = transform.getOrigin();
	//ch_leftWrist->modelMatrix = glm::translate(glm::mat4(1), glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ()));

	//transform = mRigidBodyRHand->getWorldTransform();
	//btPos = transform.getOrigin();
	//ch_rightWrist->modelMatrix = glm::translate(glm::mat4(1), glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ()));
}

void Character::FixedUpdate()
{
	mIsBat = mName == std::string("Tori");
	//printf("%s  -- ", mName.c_str());
	treatInput();
	//mTransform->GetDirection() = glm::vec3(camera.getViewMatrix() * mTransform->GetWorldMatrix() * glm::vec4(0, 0, 1, 0));
	//mTransform->GetDirection().y = 0;
	//mTransform->GetDirection() = glm::normalize(mTransform->GetDirection());
	orientTowardsEnemy();

	m_Animator->FixedUpdate();

	//if (m_Animator->mStateIDTransitionFlag >= 0 && mIsBat)
		//printf("%d\n", m_Animator->mStateIDTransitionFlag);

	if (m_Animator->mStateIDTransitionFlag == ChAnim_RELAXED_ATTACK
		|| m_Animator->mStateIDTransitionFlag == ChAnim_ENRAGED_ATTACK)
	{
		//printf(">>>>>>>>>>>>>> ATTACK <<<<<<<<<<<<<<\n");
		AttackAnimationState *attackState = ((AttackAnimationState*)(m_Animator->m_currentState));
		attackState->m_playSpeed = 0.f;
		maxAlpha = attackState->sweetSpot * 32;
		alpha = 0.f;
	}

	if(m_Animator->m_currentState->GetStateID() == ChAnim_RELAXED_ATTACK
	|| m_Animator->m_currentState->GetStateID() == ChAnim_ENRAGED_ATTACK)
	{
		AttackAnimationState *attackState = ((AttackAnimationState*)(m_Animator->m_currentState));
		
		float sweetSpotTime = attackState->sweetSpot;
		//printf("SweetSpot: %f\n", sweetSpotTime);
		if (slowTimeWarpAttack)
		{
			if (m_Animator->m_crtTime < sweetSpotTime * 0.75f)
			{
				attackState->m_playSpeed = alpha + (1 - alpha) * attackState->m_playSpeed;
				float tt = GTHTimes::Time_deltaTime / maxAlpha;
				alpha += tt;
			}
			else 
			{
				//printf("alpha = 1 (0)\n");
				alpha = 1;
				attackState->m_playSpeed = 1.f;
			}
		}
		else
		{
			//printf("alpha = 1 (1)\n");
			alpha = 1;
			attackState->m_playSpeed = 1.f;
		}
	}

	btRigidBody *rigidBody = mGameObject->GetComponentByType<Collider>()->mRigidBodies[0];
	btTransform trans = rigidBody->getWorldTransform();
	btVector3 btpos = trans.getOrigin();
	btQuaternion btrot = trans.getRotation();

	mTransform->SetPosition(glm::vec3(btpos.getX(), btpos.getY() - 4.65f, btpos.getZ()));
	mTransform->SetRotation(glm::quat(btrot.getW(), btrot.getX(), btrot.getY(), btrot.getZ()));
	
	mTransform->SetWorldMatrix( glm::translate(mTransform->GetPosition()) 
							  * glm::toMat4(mTransform->GetRotation())
							  * glm::scale(mTransform->GetScale()));

	mYpos = btpos.getY();
	if (fabs(mYpos - mPrevYpos) > 0.05 && m_Animator->m_currentState != &(m_Animator->m_jumpTakeoffState))
	{
		fallVelocity += 100.f * GTHTimes::Time_deltaTime;
	}
	else
	{
		fallVelocity = 0.f;
		m_Animator->Trigger(ChAnim_LAND);
	}
	mPrevYpos = mYpos;

	glm::mat4 inversedTrans = glm::inverse(mSkinnedMesh->GetCurrentRootTransform());
	
	mSkinnedMesh->mWorldMatrix = mTransform->GetWorldMatrix()
		* glm::scale(glm::vec3(0.0328084f, 0.0328084f, 0.0328084f))
		* inversedTrans;

	//SetLimbsPositions(translationMat, rotationMat);

	mDeltaTranslation = m_Animator->GetRootTranslation();

	mDeltaTranslation = mDeltaTranslation * 0.0328084f * 1.5f;
	float deltaDistTravelled = glm::length(glm::vec3(mDeltaTranslation.x, 0.0f, mDeltaTranslation.z));//	assert(deltaDistTravelled < 2);

																									//deltaTranslation *= 60.f * GTHTimes::Time_deltaTime;

	glm::mat4 drotation = m_Animator->GetRootRotation(deltaDistTravelled * 2.f);
	GLfloat rotX, rotY, rotZ;
	glm::extractEulerAngleXYZ(drotation, rotX, rotY, rotZ);
	//rotY *= GTHTimes::Time_deltaTime * 30.f;

	glm::vec3 worldSpaceSpeed = glm::vec3(glm::toMat4(mTransform->GetRotation()) * glm::vec4(mDeltaTranslation, 1)) / GTHTimes::Time_deltaTime;


	btVector3 rigidBodyVel = rigidBody->getLinearVelocity();
	if (m_Animator->m_currentState == &(m_Animator->m_jumpTakeoffState) && m_Animator->m_crtTime > 0.15 ||
		m_Animator->m_currentState == &(m_Animator->m_midairState))// ||
																   //m_Animator->m_currentState == &(m_Animator->m_landingState && m_Animator->m))
	{
		//mRigidBody->setLinearVelocity(btVector3(rigidBodyVel.getX(), rigidBodyVel.getY(), rigidBodyVel.getZ()));
	}
	else
		rigidBody->setLinearVelocity(btVector3(worldSpaceSpeed.x, rigidBodyVel.getY(), worldSpaceSpeed.z));

	rigidBody->setActivationState(ACTIVE_TAG);
	/*
	const btVector3 &vel = mRigidBody->getLinearVelocity();
	glm::vec3 v0 = glm::vec3(vel.getX(), vel.getY(), vel.getZ());
	glm::vec3 frictionCompensate = glm::length(worldSpaceSpeed) > 0.0001 ? 100.f * glm::normalize(worldSpaceSpeed) : glm::vec3(0, 0, 0);

	glm::vec3 deltaSpeed = (worldSpaceSpeed - v0);


	if(glm::length(deltaSpeed) > 0.001)
	{
	glm::vec3 acceleration = 9000.f * ((worldSpaceSpeed - v0) / (GTHTimes::Time_deltaTime) + frictionCompensate);
	mRigidBody->applyCentralForce(btVector3(acceleration.x, 0, acceleration.z));
	}
	*/

	float angVel = rotY / GTHTimes::Time_deltaTime;
	if (fabs(angVel) > 0.001f)
	{
		rigidBody->setAngularVelocity(btVector3(0, angVel, 0));
	}
	else
	{
		//if (isBat)
		//printf("DAMP on TORQUE\n");
		//mRigidBody->setDamping(0, 1);
		//mRigidBody->applyDamping(GTHTimes::Time_deltaTime);
		rigidBody->setAngularVelocity(btVector3(0, 0, 0));
	}

	if (timeSinceJump < 3.f)
	{
		timeSinceJump += GTHTimes::Time_deltaTime;
	}
	if (m_Animator->m_currentState == &(m_Animator->m_jumpTakeoffState) && m_Animator->m_crtTime >= 0.375f)
	{
		if (timeSinceJump > 1.5f)
		{
			timeSinceJump = 0;
			glm::vec3 jumpVec = 650000.f * glm::normalize(glm::vec3(m_moveDirection.x, 3.5f, m_moveDirection.z));
			
			printf("JUMPVEC: %5.2f %5.2f %5.2f \n", jumpVec.x, jumpVec.y, jumpVec.z);
			printf("MoveDir: %5.2f %5.2f \n", m_moveDirection.x, m_moveDirection.z);

			
			rigidBody->applyCentralImpulse(btVector3(jumpVec.x, jumpVec.y, jumpVec.z));
		}

	}
	rigidBody->applyGravity();
	/*avgSpeed = avgSpeed * .9f + glm::length(worldSpaceSpeed) * .1F;
	if (isBat)
	{
	printf("AVERAGE SPEED             : %5.2f\n"
	"accceleration             : %5.2f %5.2f %5.2f\n"
	"v0:			              : %5.2f %5.2f %5.2f\n"
	"vNEXT:                    : %5.2f %5.2f %5.2f\n"
	"angular speed             : %5.2f\n\n", avgSpeed,
	acceleration.x, acceleration.y, acceleration.z,
	v0.x, v0.y, v0.z,
	worldSpaceSpeed.x, worldSpaceSpeed.y, worldSpaceSpeed.z,
	rotY / GTHTimes::Time_deltaTime);

	//printf("Yspeed %f, Yv0, %f\n____________________________________\n", (180.f / 3.1415926 * rotY), angularv0);
	}*/
	AdjustCamera();
	//printf("]]]]\n");
}

void Character::AdjustCamera()
{
	//Camera positioning
	Character *enemy = closestEnemy();
	glm::vec3 targetPos;
	glm::vec3 blendedTargetPos;



	if (enemy && enemyEngageState)
	{
		float foo = glm::clamp(glm::distance(mTransform->GetPosition(), enemy->mTransform->GetPosition()) / ENEMY_ENGAGE_RANGE, 0.f, 1.f);
		float focusOnSelf = pow(foo, 2.f);
		targetPos = glm::vec3(mTransform->GetPosition().x * 0.5f + enemy->mTransform->GetPosition().x * 0.5f,
			mTransform->GetPosition().y * 0.5f + enemy->mTransform->GetPosition().y * 0.5f,
			mTransform->GetPosition().z * 0.5f + enemy->mTransform->GetPosition().z * 0.5f) * (1.f - focusOnSelf)
			+ mTransform->GetPosition() * focusOnSelf
			+ glm::vec3(0, 3.75, 0);
	}
	else
	{
		targetPos = mTransform->GetPosition() + glm::vec3(0, 3.75, 0);
	}

	//if (glm::distance(targetPos, oldTargetPos) > 0.0001f)
	//{
	//	blendedTargetPos = oldTargetPos + glm::clamp(GTHTimes::Time_deltaTime * 30.f, 0.f, 1.f) * (targetPos - oldTargetPos);
	//}
	//else
	//	blendedTargetPos = targetPos;

	blendedTargetPos = oldTargetPos * 0.5f + targetPos * 0.5f;
	camera.SetTargetPosition(blendedTargetPos * 0.16f + targetPos * 0.84f);
	oldTargetPos = blendedTargetPos;
	//Camera positioning
	//const glm::vec3 targetPos = glm::vec3(btpos.getX(), btpos.getY(), btpos.getZ()) + glm::vec3(0, -1, 0);
	//const glm::vec3 blendedTargetPos = targetPos * 0.05f + oldTargetPos * 0.95f;
	//camera.SetTargetPosition(glm::vec3(0,0,0));
	//oldTargetPos = blendedTargetPos;
}


///////////////////////////////////////////////////////////////////////////////

void Character::takeHit(int tag)
{
	if (HP <= 0.01f)
	{
		mAlive = false;
		m_Animator->Trigger(ChAnim_DIE);
		return;
	}

	if ((m_Animator->m_currentState == &(m_Animator->m_fightStanceGroundedState) 
	 || m_Animator->m_currentState == &(m_Animator->m_takeAmortizedHitAnimState))
		&& m_Animator->m_fightStanceGroundedState.m_blockBlendFact > 0.6)
	{
		m_Animator->Trigger(ChAnim_TAKE_AMORTIZED_HIT1);
		this->HP -= 5.f;
	}
	else{
#ifdef DEBUG_FIGHTMODE
		printf("TAKE HIT\n");
#endif
		m_Animator->Trigger(ChAnim_TAKE_HIT_1);
		m_Animator->m_takeHitAnimState.SetTag(tag);
		this->HP -= 10.f;
	}
	printf("[%s]: %f HP left\n", mName.c_str(), this->HP);
	//this->HP -= 0.05f;
}

////////////////////////////////////////////////////////////////////////////////

void Character::treatInput()
{//virtual
	//if (keyState[' '] && !midAir && jumpsCount < maxJumps && fallSpeedFactor < 5.f) { midAir = true; jumpSpeedFactor = 1.f; }
}


////////////////////////////////////////////////////////////////////////////////

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyContactResultCallback(Character* _tori, Character* _uke) : tori(_tori), uke(_uke) {}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		uke->m_Animator->Trigger(ChAnim_TAKE_HIT_1);
		return btScalar(0);
	}

	Character *tori, *uke;
};

void Character::orientTowardsEnemy()
{
#ifdef DEBUG_FIGHTMODE
	printf("\n");
#endif
	Character *closest_enemy = NULL;
	closest_enemy =	closestEnemy();
	RenderSystem &renderSys = RenderSystem::getInstance();
	if (closest_enemy)
	{
		//return;
		float distToEnemy = glm::distance(mTransform->GetPosition(), closest_enemy->mTransform->GetPosition());

		enemyEngageState = false;

		if (distToEnemy < ENEMY_ENGAGE_RANGE )
		{
			if (enemyEngageTime < 0.15f)
				enemyEngageTime += GTHTimes::Time_deltaTime;
			else
				enemyEngageState = true;
		}
		else 
		{
			if (enemyEngageTime > -0.5f)
				enemyEngageTime -= GTHTimes::Time_deltaTime;		
		}

		
		if (enemyEngageState)
		{
			m_Animator->SetFightStance(true);
			m_enemyDirection = mTransform->GetPosition() - closest_enemy->mTransform->GetPosition();
			m_enemyDirection.y = 0;
			m_enemyDirection = -glm::normalize(m_enemyDirection);
			float toriDotUke = glm::dot(glm::normalize(m_enemyDirection), glm::normalize(mTransform->GetDirection()));
			//printf("%f tdu %f\n", distToEnemy, toriDotUke);
		
			glm::vec3 boomPos;
			if (distToEnemy < 6.5 && 
				fabs(m_Animator->m_crtTime - ((AttackAnimationState*)(m_Animator->m_currentState))->sweetSpot) < (1.f / 24.f)
				&& glm::dot(m_enemyDirection, mTransform->GetDirection()) > 0.75)
			{
				mCanTestHitOpponent = true;			

				//MyContactResultCallback result(this, closest_enemy);
				char attackingLimb = m_Animator->GetAttackingLimb();
				if (dirtyTag != attackingLimb) {
					dirtyTag = attackingLimb;
					int hitTag;
					switch (attackingLimb)
					{
					case ATTACKING_LIMB_PUNCH1:
						boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightHandTransform * glm::vec4(0, 0, 0, 1));
						renderSys.SetBoomRay(mName, 1, boomPos);
						closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_PUNCH2:
						boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftHandTransform * glm::vec4(0, 0, 0, 1));
						renderSys.SetBoomRay(mName, 1, boomPos);
						closest_enemy->takeHit(HIT_REACT_LEFT);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_PUNCH3:
						boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightHandTransform * glm::vec4(0, 0, 0, 1));
						renderSys.SetBoomRay(mName, 1, boomPos);
						closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_PUNCH4:
						boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftHandTransform * glm::vec4(0, 0, 0, 1));
						renderSys.SetBoomRay(mName, 1, boomPos);
						closest_enemy->takeHit(HIT_REACT_LEFT);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_PUNCH5:
						boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftHandTransform * glm::vec4(0, 0, 0, 1));
						renderSys.SetBoomRay(mName, 1, boomPos);
						closest_enemy->takeHit(HIT_REACT_RIGHT);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_KICK1:
							boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftAnkleTransform * glm::vec4(0, 0, 0, 1));
							renderSys.SetBoomRay(mName, 1, boomPos);
							closest_enemy->takeHit(HIT_REACT_LEFT);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_KICK2:
							boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightAnkleTransform * glm::vec4(0, 0, 0, 1));
							renderSys.SetBoomRay(mName, 1, boomPos);
							closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_KICK3:
							boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftAnkleTransform * glm::vec4(0, 0, 0, 1));
							renderSys.SetBoomRay(mName, 1, boomPos);
							closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_KICK4:
							boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightAnkleTransform * glm::vec4(0, 0, 0, 1));
							renderSys.SetBoomRay(mName, 1, boomPos);
							closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_KICK5:
							boomPos = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftAnkleTransform * glm::vec4(0, 0, 0, 1));
							renderSys.SetBoomRay(mName, 1, boomPos);
							closest_enemy->takeHit(HIT_REACT_BACKOFF);
						//PhysicsgetInstance().dynamicsWorld->contactPairTest(mRigidBodyLHand, closest_enemy->mRigidBody, result);
						break;
					case ATTACKING_LIMB_NONE:
						dirtyTag = -1;
						break;
					default:
						dirtyTag = -1;

						break;
					}

				}
#ifdef DEBUG_FIGHTMODE
				else 
				{
					printf("dirty Tag == attacking Limb\n");
				}
#endif
			}
#ifdef DEBUG_FIGHTMODE
			else
			{
				if (distToEnemy > 6.5)
					printf("too big Dist\n");
				if (fabs(m_Animator->m_crtTime - ((AttackAnimationState*)(m_Animator->m_currentState))->sweetSpot) > (1.f / 24.f))
					printf("not the time\n");

				if (glm::dot(m_enemyDirection, mTransform->GetDirection()) < 0.75)
					printf("BAD QUATERNION\n");
			}
#endif
			
		}
		else
		{
			mCanTestHitOpponent = false;
			m_enemyDirection = glm::vec3(0, 0, 0);
			m_Animator->SetFightStance(false);
		}
	}
	else
	{
		enemyEngageState = false;
		enemyEngageTime = 0.0f;
		m_Animator->SetFightStance(false);
		m_enemyDirection = glm::vec3(0, 0, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ControllableCharacter::ControllableCharacter(const char *pName, bool isUke) :
	Character(pName, isUke)
{
	
	// init TPS camera

}
void ControllableCharacter::Awake()
{
	Character::Awake();
	camera.set(glm::vec3(10, 50, 3), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	SetPosition(glm::vec3(5, 30, 0), glm::vec3(-1, 0, 0));
	InputManager &mgr = InputManager::getInstance();

}
////////////////////////////////////////////////////////////////////////////////

void ControllableCharacter::treatInput()
{
	InputManager &gameController = InputManager::getInstance();
	glm::vec2 moveDir = gameController.GetXoZMovement();
	
	float mSpeed = glm::length(moveDir);

	slowTimeWarpAttack = gameController.GetXIsPressed() || gameController.keyboardControl && gameController.LMB_Pressed;
	slowTimeWarpAttack = slowTimeWarpAttack || gameController.GetSqIsPressed() || gameController.keyboardControl && gameController.RMB_Pressed;
	if (gameController.GetXDown() || gameController.keyboardControl && gameController.LMB_Down)
	{
		m_Animator->Trigger(ChAnim_RELAXED_ATTACK);
	}
	
	if (gameController.GetSqDown() || gameController.keyboardControl && gameController.RMB_Down)
	{
		m_Animator->Trigger(ChAnim_ENRAGED_ATTACK);
	}
	
	if (gameController.GetODown() || gameController.keyboardControl && gameController.freshState[' '] == 1)
	{
		m_Animator->Trigger(ChAnim_JUMP);
	}

	m_Animator->SetBlockMode(gameController.GetTriIsPressed() || gameController.MMB_Pressed);

	m_moveDirection = glm::vec3((glm::inverse(camera.getViewMatrix()) * glm::vec4(moveDir.x, 0, moveDir.y,0)));
	m_moveDirection = glm::normalize(glm::vec3(m_moveDirection.x, 0, m_moveDirection.z)) * mSpeed;
	
	if (m_moveDirection.x != m_moveDirection.x)
		m_moveDirection.x = 0.f;
	if (m_moveDirection.z != m_moveDirection.z)
		m_moveDirection.z = 0.f;

	glm::vec2 camMoved = gameController.GetCameraMovement();
	//printf("Camera Movement in Character %5.2f %5.2f\n", camMoved.x, camMoved.y);
	camera.rotateTPS_OX(camMoved.y * 50.f * GTHTimes::Time_deltaTime);
	camera.rotateTPS_OY(camMoved.x * 70.f * GTHTimes::Time_deltaTime);
	//if (gameController.GetL2Down() && !midAir && jumpsCount < maxJumps && fallSpeedFactor < 5.f) { midAir = true; jumpSpeedFactor = 1.f; }
	float moveSpeed = GTHTimes::Time_deltaTime * 40;
	Character::treatInput();
	RenderSystem &renderSys = RenderSystem::getInstance();
	//return;
	char hpKey[16];

	for (int i = 1; i <= 7; i++)
	{
		memset(hpKey, 0, 16 * sizeof(char));
		sprintf(hpKey,"HP%d", i);
		renderSys.mHUD->sprites[hpKey]->visible = false;
	}

	for (int i = 1; i <= (int) (7.f * HP / 99.5f) % 8; i++)
	{
		memset(hpKey, 0, 16 * sizeof(char));
		sprintf(hpKey, "HP%d", i);
		renderSys.mHUD->sprites[hpKey]->visible = true;
	}

}

////////////////////////////////////////////////////////////////////////////////

void ControllableCharacter::passiveMouseCallback(float yaw, float pitch)
{	
	camera.rotateTPS_OX(pitch);
	camera.rotateTPS_OY(yaw);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AICharacter::AICharacter(const char *pName, bool isUke) :
Character(pName, isUke)
{
	
}
void AICharacter::Awake()
{
	Character::Awake();
	// init TPS camera
	camera.set(glm::vec3(10, 20, 30), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	camera.disableCollisions(true);
	m_oldMoveDir = m_moveDirection = glm::vec3(0, 0, 0);
	SetPosition(glm::vec3(5, 30, 10), glm::vec3(-1, 0, 0));
}
////////////////////////////////////////////////////////////////////////////////

void AICharacter::treatInput()
{
	Character *closest = closestEnemy();
	if (closest)
	{
		//dir = closest->mTransform->GetPosition() - mTransform->GetPosition();
		m_moveDirection = closest->mTransform->GetPosition() - mTransform->GetPosition();// glm::vec3(camera.getViewMatrix() * glm::vec4(dir, 0));
		m_moveDirection.y = 0;
		m_moveDirection = 0.84f * m_oldMoveDir + 0.16f * (glm::normalize(m_moveDirection) * 0.85f);
		float dist = glm::distance(mTransform->GetPosition(), closest->mTransform->GetPosition());
		int r = rand();
		if (dist < (float)r / (float)RAND_MAX * 3 + 4)
		{
			m_moveDirection = glm::vec3(0, 0, 0);
		}
		if (dist < 5)
		{
			if ( r % 100 < 3)
			{
				if (r % 2 == 0)
					m_Animator->Trigger(ChAnim_RELAXED_ATTACK);
				else
					m_Animator->Trigger(ChAnim_ENRAGED_ATTACK);
			}

		}

	}
	else 
	{ 
		m_moveDirection = glm::vec3(0, 0, 0);
	}
	float moveSpeed = GTHTimes::Time_deltaTime * 60;
	Character::treatInput();
	m_oldMoveDir = m_moveDirection;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AICharacter::AIcallback()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AICharacter::FixedUpdate()
{
	AIcallback(); 
	Character::FixedUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControllableCharacter::FixedUpdate()
{
	Character::FixedUpdate();
	mDeadCounter += (mAlive ? 0 : GTHTimes::Time_deltaTime);
	if (mDeadCounter >= 3.f)
	{
		ObjectFactory &of = ObjectFactory::getInstance();
		of.ResetGame();
	}
	HandleMotionTrace();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Character::HandleMotionTrace()
{
	glm::vec3 aCrt, bCrt; 
	switch (m_Animator->m_currentState->GetAttackingLimb())
	{
	case ATTACKING_LIMB_PUNCH2:
	case ATTACKING_LIMB_PUNCH4:
	case ATTACKING_LIMB_PUNCH5:
		mMotionTrace.active = true;
		aCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftForearmTransform * glm::vec4(0, 0, 0, 1));
		bCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftHandTransform * glm::vec4(0, 0, 0, 1));
		mMotionTrace.AddStep(aCrt, bCrt);
		break;
	case ATTACKING_LIMB_PUNCH1:
	case ATTACKING_LIMB_PUNCH3:
		mMotionTrace.active = true;
		aCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightForearmTransform * glm::vec4(0, 0, 0, 1));
		bCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightHandTransform * glm::vec4(0, 0, 0, 1));
		mMotionTrace.AddStep(aCrt, bCrt);
		break;
	case ATTACKING_LIMB_KICK1:
	case ATTACKING_LIMB_KICK3:
	case ATTACKING_LIMB_KICK5:
		mMotionTrace.active = true;
		aCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftShinTransform * glm::vec4(0, 0, 0, 1));
		bCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtLeftAnkleTransform * glm::vec4(0, 0, 0, 1));
		mMotionTrace.AddStep(aCrt, bCrt);
		break;
	case ATTACKING_LIMB_KICK2:
	case ATTACKING_LIMB_KICK4:
		mMotionTrace.active = true;
		aCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightShinTransform * glm::vec4(0, 0, 0, 1));
		bCrt = glm::vec3(mSkinnedMesh->mWorldMatrix * mSkinnedMesh->m_crtRightAnkleTransform * glm::vec4(0, 0, 0, 1));
		mMotionTrace.AddStep(aCrt, bCrt);
		break;
	default:
		mMotionTrace.Reset();
		mMotionTrace.active = false;
		return;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Character::MotionTrace::MotionTrace()
{
	RenderSystem &renderSys = RenderSystem::getInstance();
	renderSys.mMotionTraces.push_back(this);
	glGenBuffers(MT_NUM_BUFFERS, vBuffers);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);
}

Character::MotionTrace::~MotionTrace()
{
	glDeleteBuffers(MT_NUM_BUFFERS, vBuffers);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}

void Character::MotionTrace::Reset()
{
	aPoints.clear();
	bPoints.clear();
}

void Character::MotionTrace::AddStep(glm::vec3 crtFrameA, glm::vec3 crtFrameB)
{
	int sz = aPoints.size();
	//PREPROCESS QUEUE
	if (sz > 0){
		aPoints.push_back(aPoints[sz - 1]);
		bPoints.push_back(bPoints[sz - 1]);
	}
	else
	{
		aPoints.push_back(crtFrameA);
		bPoints.push_back(crtFrameB);
		
	}
	aPoints.push_back(crtFrameA);
	bPoints.push_back(crtFrameB);

	if (aPoints.size() > 2 * maxTraceSteps)
	{
		aPoints.pop_front();
		aPoints.pop_front();
		bPoints.pop_front();
		bPoints.pop_front();
	}//end PREPROCESS
	
	std::vector<glm::vec3> positions;
	std::vector<float> texcoords;
	std::vector<float> opacities;
	std::vector<GLuint> indices;
	//COMPUTE MOTION TRACE
	int ocount = 0, tricount = 0;
	sz = aPoints.size();
	for (int i = 0; i < sz; i+=2)
	{
		positions.push_back(aPoints[i]);
		positions.push_back(bPoints[i]);
		positions.push_back(aPoints[i + 1]);
		positions.push_back(bPoints[i + 1]);
		
		texcoords.push_back(0);
		texcoords.push_back(1);
		texcoords.push_back(0);
		texcoords.push_back(1);

		opacities.push_back((float)ocount / (float)sz);
		opacities.push_back((float)ocount / (float)sz);
		ocount++;
		opacities.push_back((float)ocount / (float)sz);
		opacities.push_back((float)ocount / (float)sz);

		indices.push_back(tricount);
		indices.push_back(tricount + 1);
		indices.push_back(tricount + 2);
		indices.push_back(tricount + 1);
		indices.push_back(tricount + 3);
		indices.push_back(tricount + 2);
		tricount += 4;
	}//end COMPUTE MOTION TRACE
	count = indices.size();
	//SEND DATA TO GPU
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_POS]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0][0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_TEXC]);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_OPACITY]);
	glBufferData(GL_ARRAY_BUFFER, opacities.size() * sizeof(float), &opacities[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Character::MotionTrace::Render(Shader *shader)
{
	const int POS_LOCATION = shader->GetAttributeLocation("vPosition");
	const int TEXC_LOCATION = shader->GetAttributeLocation("vTexCoord");
	const int OPAC_LOCATION = shader->GetAttributeLocation("vOpacity");

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_POS]);
	glEnableVertexAttribArray(POS_LOCATION);
	glVertexAttribPointer(POS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_TEXC]);
	glEnableVertexAttribArray(TEXC_LOCATION);
	glVertexAttribPointer(TEXC_LOCATION, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffers[MT_OPACITY]);
	glEnableVertexAttribArray(OPAC_LOCATION);
	glVertexAttribPointer(OPAC_LOCATION, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}