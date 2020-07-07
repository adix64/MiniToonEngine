#pragma once
#include <Component/SkinnedMeshRenderer.h>
#include <Component/CharacterAnimation/CharacterAnimator.h>
#include <Component/MeshRenderer.h>
#include <Component/CollidableCamera.h>
#include <Component/CameraExtensions.h>
#include <GlobalTime.h>
#include <PhysicsSystem.h>
#include <InputManager.h>
#include <GameObject.h>
#include <Component/Collider.h>
#include <deque>
class Character : public Component
{
public:
	struct MotionTrace
	{
		bool active = false;
		enum bufferTypes {MT_POS, MT_TEXC, MT_OPACITY, MT_NUM_BUFFERS};
		GLuint vBuffers[MT_NUM_BUFFERS] = {0,0,0}, ibo = 0, vao = 0, count = 0;
		std::deque<glm::vec3> aPoints, bPoints;
		int maxTraceSteps = 12;
		MotionTrace();
		~MotionTrace();
		void Reset();
		void AddStep(glm::vec3 crtFrameA, glm::vec3 crtFrameB);
		void Render(Shader *shader);
	};
	MotionTrace mMotionTrace;
	class MyKinematicMotionState : public btMotionState {
	public:
		MyKinematicMotionState(const btTransform &initialpos) { mPos1 = initialpos; }
		virtual ~MyKinematicMotionState() { }
		virtual void getWorldTransform(btTransform &worldTrans) const { worldTrans = mPos1; }
		void setKinematicPos(btTransform &currentPos) { mPos1 = currentPos; }
		virtual void setWorldTransform(const btTransform &worldTrans) { }
	protected:
		btTransform mPos1;
	};


	bool mAlive = true;
	float mDeadCounter = 0;
	float alpha = 0.f, maxAlpha = 0.5f;
	bool mIsBat;
	bool slowTimeWarpAttack = false;
	Transform *mTransform;
	float timeSinceJump = 5.f;
	float enemyEngageTime = 0.0f;
	bool enemyEngageState = false;
	float avgSpeed;
	bool mCanTestHitOpponent;
	bool mIsUke;
	int dirtyTag = -1;
	float mYpos = 0.f, mPrevYpos = 0.f;
	float fallVelocity = 0.f;
	float HP = 100.f;
	//Bullet Physics
	//btCollisionShape* mColShape = NULL;
	//btCollisionShape *mColShapeLAnkle = NULL;
	//btCollisionShape *mColShapeRAnkle = NULL;
	//btCollisionShape *mColShapeLHand = NULL;
	//btCollisionShape *mColShapeRHand = NULL;

	//btRigidBody *mRigidBody = NULL;
	//btRigidBody *mRigidBodyLAnkle = NULL;
	//btRigidBody *mRigidBodyRAnkle = NULL;
	//btRigidBody *mRigidBodyLHand = NULL;
	//btRigidBody *mRigidBodyRHand = NULL;

	//MyKinematicMotionState *mMotionStateLAnkle = NULL;
	//MyKinematicMotionState *mMotionStateRAnkle = NULL;
	//MyKinematicMotionState *mMotionStateLHand = NULL;
	//MyKinematicMotionState *mMotionStateRHand = NULL;

	void InitWorldTransforms();
	void SetPosition(glm::vec3 &pos, glm::vec3 &lookDir);
	void ResetKinematicRigidBody(btRigidBody *&rigidBody, btCollisionShape *&colShape, MyKinematicMotionState *&motionState, glm::vec3 &newPos);
	void ResetCapsuleRigidBody(btRigidBody *&rigidBody, btCollisionShape *&colShape, glm::vec3 &newPos, float _mass = 10);
	///Bullet

	glm::vec3 oldTargetPos;
	
	glm::vec3 mDeltaTranslation;

	std::string mName;
	
	SkinnedMeshRenderer *mSkinnedMesh = NULL;
	Collider *mCollider = NULL;
	//MeshRenderer *ch_bounding_box;
	//MeshRenderer *ch_leftWrist, *ch_rightWrist, *ch_leftAnkle, *ch_rightAnkle;

	CharacterAnimator *m_Animator = NULL;

	glm::vec3 m_moveDirection;
	glm::vec3 m_enemyDirection;

	void SetLimbsPositions(glm::mat4 &trans, glm::mat4 &rot);
	CollidableCamera camera;	
	std::vector<Character*> enemies;

public:
	Character(const char *pName, bool isUke = false);

//	void set_bs_mesh(BlendShapeMesh *_bs_mesh);
	
	void AddEnemy(Character *enemy);

	Character *closestEnemy();

	SkinnedMeshRenderer *getSkinnedMesh();

	virtual void Awake();

	virtual void FixedUpdate();

	void takeHit(int tag);

	virtual void treatInput();

	void orientTowardsEnemy();

	void AdjustCamera();

	void HandleMotionTrace();

	friend class PhysicsEngine;
};

class ControllableCharacter : public Character {
public:

	ControllableCharacter(const char *pName, bool isUke = false);
	void Awake();
	void treatInput();

	void FixedUpdate();
	void passiveMouseCallback(float yaw, float pitch);
};

class AICharacter : public Character {
	glm::vec3 m_oldMoveDir;
public:
	Camera fps_cam;
	AICharacter(const char *pName, bool isUke = false);
	void Awake();

	void treatInput();
	
	void AIcallback();

	void FixedUpdate();
};

