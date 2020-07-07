#pragma once
#include <Component/CharacterAnimation/ChAnimDefines.h>
#include <Component/CharacterAnimation/WalkRunAnimationState.h>
#include <Component/CharacterAnimation/FightStanceAnimationState.h>
#include <Component/CharacterAnimation/WalkRunAnimationState.h>
#include <Component/CharacterAnimation/AttackAnimationState.h>

#include <vector>
#include <Component/SkinnedMeshRenderer.h>
#include <Component/CharacterAnimation/SkeletalAnimation.h>
#include <InputManager.h>
#include <GlobalTime.h>
#include <Component/Component.h>
class BaseAnimationState;


class CharacterAnimator : public Component
{
public:
	CharacterAnimator();// {}
	void Awake();
	void FixedUpdate();
	void ComputeDirectionVectors();
	void HandleTimeoutTransitions();
	void HandleTriggerTransitions();
	uint16_t GetStateID()
	{
		return m_currentState->GetStateID();
	}
	char GetAttackingLimb();
	void SetFightStance(bool state);
	void Trigger(uint16_t action);
//private:
	void CreateAnimaFSM(const glm::vec3 *fwdVecPtr, glm::vec3 *moveDirPtr, glm::vec3 *enemyDirPtr){
		CreateAnimaStates(fwdVecPtr, moveDirPtr, enemyDirPtr);
		CreateStateLinks();
		m_currentState = (BaseAnimationState *)&m_defaultGroundedState;
		m_crtTime = m_prevTime = 0; 
        blendToNextState = false;
	}
	void CreateAnimaStates(const glm::vec3 *fwdVecPtr, glm::vec3 *moveDirPtr, glm::vec3 *enemyDirPtr);
	void CreateStateLinks();
	
	void SetBlockMode(bool);
//private:
	BaseAnimationState *m_currentState, *m_prevFrameState;
	BaseAnimationState *m_previousState;
	BaseAnimationState m_startState;

	WalkRunAnimationState m_defaultGroundedState;
	FightStanceLocomotionAnimationState m_fightStanceGroundedState;
	//attackStates
	AttackAnimationState m_relaxedAttack_State1;
	AttackAnimationState m_relaxedAttack_State2;
	AttackAnimationState m_relaxedAttack_State3;
	AttackAnimationState m_relaxedAttack_State4;
	AttackAnimationState m_relaxedAttack_State5;

	AttackAnimationState m_enragedAttack_State1;
	AttackAnimationState m_enragedAttack_State2;
	AttackAnimationState m_enragedAttack_State3;
	AttackAnimationState m_enragedAttack_State4;
	AttackAnimationState m_enragedAttack_State5;

	HitReactAnimState m_takeHitAnimState;
	AttackAnimationState m_blockHitAnimState;
	HitReactAnimState m_dieState;
	HitReactAnimState m_deadState;

	AttackAnimationState m_jumpTakeoffState, m_midairState, m_landingState;


	TakeAmortizedHitAnimState m_takeAmortizedHitAnimState;
	float wr_speed = 1.f;
	float blendSum = 0.f;
	std::string mName;
	bool mIsBat;
	float m_crtTime, m_prevTime;
	bool zeroRootTransform;
	bool isFightStance;
	float timeToBlend;
	bool blendToNextState;
	glm::mat4 mAnimRotation;
	float moveDirDotProduct, enemyDotProduct;
	glm::vec3 moveDirCrossProduct, enemyCrossProduct;
	std::vector<std::pair<float, CharacterAnimation*> > mBlendScheme;
	int mStateIDTransitionFlag = -1;
	SkinnedMeshRenderer* mSkinnedMesh;
	const glm::vec3 *m_characterFwdVecPtr;
	glm::vec3 *m_directionVecPtr, *m_enemyDirectionPtr;// , *enemyDirPtr;
//	InputManager *m_gameController;
	
	glm::vec3 GetRootTranslation() { return _GetRootTranslation(mBlendScheme, m_crtTime); }
	glm::mat4 GetRootRotation();
	glm::mat4 GetRootRotation(float deltaTrans);

	glm::vec3 _GetRootTranslation(std::vector<std::pair<float, CharacterAnimation*> > &blendScheme, float t);
	glm::mat4 _GetRootRotation(std::vector<std::pair<float, CharacterAnimation*> > &blendScheme, float t);

	std::unordered_map<CharacterAnimation*, std::pair<glm::vec3, float> > prevRootPositions, crtRootPositions;
	std::unordered_map<CharacterAnimation*, std::pair<glm::quat,float> >  prevRootRotations, crtRootRotations;
};
