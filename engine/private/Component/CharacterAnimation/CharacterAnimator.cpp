#include <Component/CharacterAnimation/CharacterAnimator.h>
#include <Component/CharacterAnimation/AnimationTransition.h>
#include "ResourceManager.h"
#include <typeinfo>
#include "SoundManager.h"
#include <GameObject.h>
#include <GameObjects\Character.h>
#include <AnimationSystem.h>
CharacterAnimator::CharacterAnimator()
{
	AnimationSystem &animSys = AnimationSystem::getInstance();
	animSys.mAnimators.push_back(this);
}
void CharacterAnimator::Awake()
{
	mName = mGameObject->mName;// +"Animator";
	mSkinnedMesh = mGameObject->GetComponentByType<SkinnedMeshRenderer>();
	Character *charScript = mGameObject->GetComponentByType<Character>();
	CreateAnimaFSM(&(mGameObject->mTransform.GetDirection()), &(charScript->m_moveDirection), &(charScript->m_enemyDirection));
	isFightStance = zeroRootTransform = false;

}
void CharacterAnimator::SetBlockMode(bool isBlocking)
{	
	SetFightStance(true);
	m_fightStanceGroundedState.SetBlockMode(isBlocking);
}
	
//////////////////////////////////////////////////////////////////////////////

void CharacterAnimator::CreateAnimaStates(const glm::vec3 *fwdVecPtr, glm::vec3 *moveDirPtr, glm::vec3 *enemyDirPtr)
{
	ResourceManager& resMgr = ResourceManager::getInstance();
	m_defaultGroundedState = WalkRunAnimationState(ChAnim_DEFAULT_GROUNDED, resMgr.GetAnimNFO(mName, ChAnim_RELAXED_BREATHE_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_WALK_FWD_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_WALK_LEFT_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_WALK_RIGHT_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_RUN_FWD_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_RUN_LEFT_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_RUN_RIGHT_FILE).fileName,
		fwdVecPtr, moveDirPtr);
	m_defaultGroundedState.mIsCycle = true;
	m_fightStanceGroundedState = FightStanceLocomotionAnimationState(ChAnim_FSTANCE_GROUNDED, resMgr.GetAnimNFO(mName, ChAnim_BREATHE_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_FWDL_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_FWDR_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_LEFTFWD_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_RIGHTFWD_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_LEFT_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_RIGHT_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_LEFTBACK_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_RIGHTBACK_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_BACKL_FILE).fileName, resMgr.GetAnimNFO(mName, ChAnim_FSTANCE_MOVE_BACKR_FILE).fileName,
		resMgr.GetAnimNFO(mName, ChAnim_BLOCK_FILE).fileName,
		fwdVecPtr, moveDirPtr, enemyDirPtr);
	m_fightStanceGroundedState.mIsCycle = true;

	m_relaxedAttack_State1 = AttackAnimationState(ChAnim_RELAXED_ATTACK, ChAnim_RELAXED_ATTACK_1_FILE, mName);
	m_relaxedAttack_State1.SetAttackingLimb(ATTACKING_LIMB_PUNCH1); m_relaxedAttack_State1.stateTag = 0;

	m_relaxedAttack_State2 = AttackAnimationState(ChAnim_RELAXED_ATTACK, ChAnim_RELAXED_ATTACK_2_FILE, mName);
	m_relaxedAttack_State2.SetAttackingLimb(ATTACKING_LIMB_PUNCH2); m_relaxedAttack_State2.stateTag = 1;

	m_relaxedAttack_State3 = AttackAnimationState(ChAnim_RELAXED_ATTACK, ChAnim_RELAXED_ATTACK_3_FILE, mName);
	m_relaxedAttack_State3.SetAttackingLimb(ATTACKING_LIMB_PUNCH3); m_relaxedAttack_State3.stateTag = 2;

	m_relaxedAttack_State4 = AttackAnimationState(ChAnim_RELAXED_ATTACK, ChAnim_RELAXED_ATTACK_4_FILE, mName);
	m_relaxedAttack_State4.SetAttackingLimb(ATTACKING_LIMB_PUNCH4); m_relaxedAttack_State4.stateTag = 3;

	m_relaxedAttack_State5 = AttackAnimationState(ChAnim_RELAXED_ATTACK, ChAnim_RELAXED_ATTACK_5_FILE, mName);
	m_relaxedAttack_State5.SetAttackingLimb(ATTACKING_LIMB_PUNCH5); m_relaxedAttack_State5.stateTag = 4;


	m_enragedAttack_State1 = AttackAnimationState(ChAnim_ENRAGED_ATTACK, ChAnim_ENRAGED_ATTACK_1_FILE, mName);
	m_enragedAttack_State1.SetAttackingLimb(ATTACKING_LIMB_KICK1); m_enragedAttack_State1.stateTag = 5;

	m_enragedAttack_State2 = AttackAnimationState(ChAnim_ENRAGED_ATTACK, ChAnim_ENRAGED_ATTACK_2_FILE, mName);
	m_enragedAttack_State2.SetAttackingLimb(ATTACKING_LIMB_KICK2); m_enragedAttack_State2.stateTag = 6;

	m_enragedAttack_State3 = AttackAnimationState(ChAnim_ENRAGED_ATTACK, ChAnim_ENRAGED_ATTACK_3_FILE, mName);
	m_enragedAttack_State3.SetAttackingLimb(ATTACKING_LIMB_KICK3); m_enragedAttack_State3.stateTag = 7;

	m_enragedAttack_State4 = AttackAnimationState(ChAnim_ENRAGED_ATTACK, ChAnim_ENRAGED_ATTACK_4_FILE, mName);
	m_enragedAttack_State4.SetAttackingLimb(ATTACKING_LIMB_KICK4); m_enragedAttack_State4.stateTag = 8;

	m_enragedAttack_State5 = AttackAnimationState(ChAnim_ENRAGED_ATTACK, ChAnim_ENRAGED_ATTACK_5_FILE, mName);
	m_enragedAttack_State5.SetAttackingLimb(ATTACKING_LIMB_KICK5); m_enragedAttack_State5.stateTag = 9;

	std::vector<std::string> takehitfiles;
	takehitfiles.push_back(resMgr.GetAnimNFO(mName, ChAnim_TAKE_HIT_1_FILE).fileName);
	takehitfiles.push_back(resMgr.GetAnimNFO(mName, ChAnim_TAKE_HIT_2_FILE).fileName);
	takehitfiles.push_back(resMgr.GetAnimNFO(mName, ChAnim_TAKE_HIT_3_FILE).fileName);
	m_takeHitAnimState = HitReactAnimState(ChAnim_TAKE_HIT_1, takehitfiles);
												//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!>>>>>> below...
	m_takeAmortizedHitAnimState = TakeAmortizedHitAnimState(ChAnim_TAKE_AMORTIZED_HIT1, m_takeHitAnimState.hitAnims[0], (CharacterAnimation*)m_fightStanceGroundedState.m_blockAnim);
	
	std::vector<std::string> foo;
	foo.push_back(resMgr.GetAnimNFO(mName, ChAnim_DIE_1_FILE).fileName);
	m_dieState = HitReactAnimState(ChAnim_DIE, foo);

	std::vector<std::string> bar;
	bar.push_back(resMgr.GetAnimNFO(mName, ChAnim_DEAD_1_FILE).fileName);
	m_deadState = HitReactAnimState(ChAnim_DEAD, bar);

	m_characterFwdVecPtr = fwdVecPtr;  m_directionVecPtr = moveDirPtr; m_enemyDirectionPtr = enemyDirPtr;

	m_jumpTakeoffState = AttackAnimationState(ChAnim_JUMP, ChAnim_JUMP_FILE, mName);
	m_midairState = AttackAnimationState(ChAnim_MIDAIR, ChAnim_MIDAIR_FILE, mName);
	m_midairState.mIsCycle = true;
	m_landingState = AttackAnimationState(ChAnim_LAND,  ChAnim_LAND_FILE, mName);


	//////////-------------sounds-------------------------------


	m_relaxedAttack_State1.SetSoundID(woosh1);
	m_relaxedAttack_State2.SetSoundID(woosh2);
	m_relaxedAttack_State3.SetSoundID(woosh3);
	m_relaxedAttack_State4.SetSoundID(woosh4);
	m_relaxedAttack_State5.SetSoundID(woosh5);
	m_enragedAttack_State1.SetSoundID(woosh6);
	m_enragedAttack_State2.SetSoundID(woosh7);
	m_enragedAttack_State3.SetSoundID(woosh6);
	m_enragedAttack_State4.SetSoundID(woosh5);
	m_enragedAttack_State5.SetSoundID(woosh4);

	m_takeHitAnimState.SetSoundID(hitSID);
}

//////////////////////////////////////////////////////////////////////////////

void CharacterAnimator::CreateStateLinks()
{//Default next states
	m_defaultGroundedState.SetDefaultNextState(&m_defaultGroundedState);
	m_fightStanceGroundedState.SetDefaultNextState(&m_defaultGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_relaxedAttack_State1.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_relaxedAttack_State2.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_relaxedAttack_State3.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_relaxedAttack_State4.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_relaxedAttack_State5.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!

	m_enragedAttack_State1.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_enragedAttack_State2.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_enragedAttack_State3.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_enragedAttack_State4.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	m_enragedAttack_State5.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!

	m_takeHitAnimState.SetDefaultNextState(&m_fightStanceGroundedState);
	m_takeAmortizedHitAnimState.SetDefaultNextState(&m_fightStanceGroundedState);
	
	m_dieState.SetDefaultNextState(&m_deadState);
	m_deadState.SetDefaultNextState(&m_deadState);

	m_jumpTakeoffState.SetDefaultNextState(&m_midairState);
	m_midairState.SetDefaultNextState(&m_midairState);

	m_defaultGroundedState.AddTransition(AnimationTransition(&m_jumpTakeoffState, 0.005f, 0.05f));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_jumpTakeoffState, 0.02f, 0.05f));
	m_midairState.AddTransition(AnimationTransition(&m_landingState, 0.005f, 0.005f));
	m_landingState.SetDefaultNextState(&m_defaultGroundedState);


	// default grounded state
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05f));
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_fightStanceGroundedState, 0.08f, 0.05));
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_relaxedAttack_State1, 0.1f, 0.2f));
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_enragedAttack_State1, 0.1f, 0.2f));
	m_defaultGroundedState.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	// fight stance grounded state
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_defaultGroundedState, 0.08f, 0.05));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_relaxedAttack_State1, 0.08f, 0.05));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_enragedAttack_State1, 0.08f, 0.05));
	m_fightStanceGroundedState.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ResourceManager &resMgr = ResourceManager::getInstance();
	ResourceManager::CharAnimNFO p1 = resMgr.GetAnimNFO(mName, ChAnim_RELAXED_ATTACK_1_FILE);
	ResourceManager::CharAnimNFO p2 = resMgr.GetAnimNFO(mName, ChAnim_RELAXED_ATTACK_2_FILE);
	ResourceManager::CharAnimNFO p3 = resMgr.GetAnimNFO(mName, ChAnim_RELAXED_ATTACK_3_FILE);
	ResourceManager::CharAnimNFO p4 = resMgr.GetAnimNFO(mName, ChAnim_RELAXED_ATTACK_4_FILE);
	ResourceManager::CharAnimNFO p5 = resMgr.GetAnimNFO(mName, ChAnim_RELAXED_ATTACK_5_FILE);

	ResourceManager::CharAnimNFO k1 = resMgr.GetAnimNFO(mName, ChAnim_ENRAGED_ATTACK_1_FILE);
	ResourceManager::CharAnimNFO k2 = resMgr.GetAnimNFO(mName, ChAnim_ENRAGED_ATTACK_2_FILE);
	ResourceManager::CharAnimNFO k3 = resMgr.GetAnimNFO(mName, ChAnim_ENRAGED_ATTACK_3_FILE);
	ResourceManager::CharAnimNFO k4 = resMgr.GetAnimNFO(mName, ChAnim_ENRAGED_ATTACK_4_FILE);
	ResourceManager::CharAnimNFO k5 = resMgr.GetAnimNFO(mName, ChAnim_ENRAGED_ATTACK_5_FILE);

		//relaxed attack state 1
	float attackDuration;
	attackDuration = m_relaxedAttack_State1.m_animation->GetDuration();
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_fightStanceGroundedState, p1.relevantTime, p1.maxFadeInTime)); ///>>lastaparamworng
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_relaxedAttack_State2, p1.minMandatoryTime, p2.maxFadeInTime));
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_enragedAttack_State1, p1.minMandatoryTime, k1.maxFadeInTime));
	m_relaxedAttack_State1.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//relaxed attack state 2
	attackDuration = m_relaxedAttack_State2.m_animation->GetDuration();
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_fightStanceGroundedState, p2.relevantTime, p2.maxFadeInTime));///>>lastparamworng
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_relaxedAttack_State3, p2.minMandatoryTime, p3.maxFadeInTime));
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_enragedAttack_State2, p2.minMandatoryTime, k2.maxFadeInTime));
	m_relaxedAttack_State2.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//relaxed attack state 3
	attackDuration = m_relaxedAttack_State3.m_animation->GetDuration();
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_fightStanceGroundedState, p3.relevantTime, p3.maxFadeInTime)); ///>>>lastparamwrong
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_relaxedAttack_State4, p3.minMandatoryTime, p4.maxFadeInTime));
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_enragedAttack_State3, p3.minMandatoryTime, k3.maxFadeInTime));
	m_relaxedAttack_State3.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));


	//relaxed attack state 4
	attackDuration = m_relaxedAttack_State4.m_animation->GetDuration();
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.2));
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_fightStanceGroundedState, p4.relevantTime, p4.maxFadeInTime));//lastparamwrong
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_relaxedAttack_State5, p4.minMandatoryTime, p5.maxFadeInTime));
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_enragedAttack_State4, p4.minMandatoryTime, k4.maxFadeInTime));
	m_relaxedAttack_State4.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));


	//relaxed attack state 5
	attackDuration = m_relaxedAttack_State5.m_animation->GetDuration();
	m_relaxedAttack_State5.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.2));
	m_relaxedAttack_State5.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_relaxedAttack_State5.AddTransition(AnimationTransition(&m_fightStanceGroundedState, p5.relevantTime, 0.1));
	m_relaxedAttack_State5.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//enraged attack state 1
	attackDuration = m_enragedAttack_State1.m_animation->GetDuration();
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.0f, 0.05f));
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_fightStanceGroundedState, k1.relevantTime, 0.05));
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_enragedAttack_State2, k1.minMandatoryTime, k2.maxFadeInTime));
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_relaxedAttack_State1, k1.minMandatoryTime, p1.maxFadeInTime));
	m_enragedAttack_State1.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//enraged attack state 2
	attackDuration = m_enragedAttack_State2.m_animation->GetDuration();
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.0f, 0.05f));
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_fightStanceGroundedState, k2.relevantTime, 0.05));
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_enragedAttack_State3, k2.minMandatoryTime, k3.maxFadeInTime));
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_relaxedAttack_State2, k2.minMandatoryTime, p2.maxFadeInTime));
	m_enragedAttack_State2.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//enraged attack state 3
	attackDuration = m_enragedAttack_State3.m_animation->GetDuration();
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_fightStanceGroundedState, k3.relevantTime, 0.05));
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_enragedAttack_State4, k3.minMandatoryTime, k4.maxFadeInTime));
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_relaxedAttack_State3, k3.minMandatoryTime, p3.maxFadeInTime));
	m_enragedAttack_State3.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//enraged attack state 4
	attackDuration = m_enragedAttack_State4.m_animation->GetDuration();
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_fightStanceGroundedState, k4.relevantTime, 0.05));
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_enragedAttack_State5, k4.minMandatoryTime, k5.maxFadeInTime));
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_relaxedAttack_State4, k4.minMandatoryTime, p4.maxFadeInTime ));
	m_enragedAttack_State4.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	//enraged attack state 5
	attackDuration = m_enragedAttack_State5.m_animation->GetDuration();
	m_enragedAttack_State5.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005f, 0.05));
	m_enragedAttack_State5.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, 0.02f, 0.05f));
	m_enragedAttack_State5.AddTransition(AnimationTransition(&m_fightStanceGroundedState, k5.relevantTime, 0.05));
	m_enragedAttack_State5.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_takeHitAnimState.AddTransition(AnimationTransition(&m_fightStanceGroundedState, 0.6, 0.05));
	m_takeHitAnimState.AddTransition(AnimationTransition(&m_takeHitAnimState, 0.005, 0.005));///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_takeHitAnimState.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, m_takeHitAnimState.GetDuration() * 0.3f, 0.05f));
	m_takeHitAnimState.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));


	m_takeAmortizedHitAnimState.AddTransition(AnimationTransition(&m_fightStanceGroundedState, 0.6, 0.05));
	m_takeAmortizedHitAnimState.AddTransition(AnimationTransition(&m_takeHitAnimState, m_takeHitAnimState.GetDuration() * 0.4f, 0.05f));///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_takeAmortizedHitAnimState.AddTransition(AnimationTransition(&m_takeAmortizedHitAnimState, m_takeHitAnimState.GetDuration() * 0.3f, 0.05f));
	m_takeAmortizedHitAnimState.AddTransition(AnimationTransition(&m_dieState, 0.05f, 0.05f));

	m_currentState = &m_defaultGroundedState;	
}

//////////////////////////////////////////////////////////////////////////////

char CharacterAnimator::GetAttackingLimb()
{
	return m_currentState->GetAttackingLimb();
}

//////////////////////////////////////////////////////////////////////////////

void CharacterAnimator::SetFightStance(bool state)
{
	isFightStance = state;
}

//////////////////////////////////////////////////////////////////////////////
void CharacterAnimator::ComputeDirectionVectors()
{
	//mAnimRotation = _GetRootRotation(mBlendScheme, m_crtTime); //alex dinu
	//*m_characterFwdVecPtr = glm::vec3(mAnimRotation * glm::vec4(*m_characterFwdVecPtr, 0));

	moveDirDotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_directionVecPtr));
	moveDirCrossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_directionVecPtr));

	enemyDotProduct = 0.0f;
	enemyCrossProduct = glm::vec3(0, 0, 0);

	if (fabs(m_enemyDirectionPtr->x) > 0.01 || fabs(m_enemyDirectionPtr->z) > 0.01)
	{
		enemyDotProduct = glm::dot(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirectionPtr));
		enemyCrossProduct = glm::cross(glm::normalize(*m_characterFwdVecPtr), glm::normalize(*m_enemyDirectionPtr));
	}
}
void CharacterAnimator::HandleTimeoutTransitions()
{
	if (blendToNextState)
		return;
	if (m_currentState == &m_midairState || m_currentState == &m_landingState)
	{
		if(m_crtTime >= m_currentState->GetDuration())
		{	
			m_crtTime = m_prevTime = 0.0f;
			zeroRootTransform = true;

				m_currentState = m_currentState->GetDefaultNextState();
			mStateIDTransitionFlag = -1;
		}
		return;
	}
	
#define TIMEOUT_BLEND_TIME std::min(0.1f, m_currentState->GetDuration()) - 0.0001f
	
	if (m_currentState->mIsCycle)
	{
		if (m_currentState == m_currentState->GetDefaultNextState()) {
			//printf("m_duration:: %f\n", m_currentState->GetDuration());
			// DEFAULT STATE
			if (m_crtTime >= m_currentState->GetDuration()) {
				m_crtTime = m_prevTime = 0.0f;
				zeroRootTransform = true;
				
				m_currentState = m_currentState->GetDefaultNextState();
				mStateIDTransitionFlag = -1;
			}
		}
		else
		{
			if (m_crtTime >= m_currentState->GetDuration() - TIMEOUT_BLEND_TIME) {
				m_currentState->Reset();
				timeToBlend = TIMEOUT_BLEND_TIME - 0.01f;
				m_previousState = m_currentState;
				m_currentState = (BaseAnimationState *)m_currentState->GetDefaultNextState();

				SoundManager &soundMgr = SoundManager::getInstance();
				soundMgr.PlaySound(m_currentState->GetSoundID());
				blendToNextState = true;
				
				m_prevTime = m_crtTime;
				m_crtTime = 0.f;

				zeroRootTransform = true;
				mStateIDTransitionFlag = m_currentState->GetStateID();
			}
		}
	}
	else
	{
		if (m_crtTime >= m_currentState->GetDuration() - TIMEOUT_BLEND_TIME) {
			m_currentState->Reset();
			timeToBlend = TIMEOUT_BLEND_TIME - 0.01f;
			m_previousState = m_currentState;
			m_currentState = (BaseAnimationState *)m_currentState->GetDefaultNextState();

			SoundManager &soundMgr = SoundManager::getInstance();
			soundMgr.PlaySound(m_currentState->GetSoundID());
			blendToNextState = true;

			m_prevTime = m_crtTime;
			m_crtTime = 0.f;

			zeroRootTransform = true;
			mStateIDTransitionFlag = m_currentState->GetStateID();
		}
	}

}

void CharacterAnimator::HandleTriggerTransitions()
{
	blendSum = 0;
	mStateIDTransitionFlag = -1;
	if (blendToNextState)
	{//HANDLE BLENDING TO NEXT STATE IF IN COURSE
		float blendFactToNext = m_crtTime / timeToBlend;
		{
			m_previousState->CalcBlendFactors(wr_speed, moveDirDotProduct, moveDirCrossProduct, enemyDotProduct, enemyCrossProduct);
			mBlendScheme = m_previousState->getBlendScheme();


			for (uint16_t i = 0; i < mBlendScheme.size(); i++)
			{
				blendSum += mBlendScheme[i].first;
				float ff = (1 - blendFactToNext) * mBlendScheme[i].first / blendSum;
				ff = glm::clamp(ff, 0.f, 1.f);
				mSkinnedMesh->BlendWithAnimAt(mBlendScheme[i].second->GetSkeletalAnim()->m_animaBoneStruct, m_prevTime, ff);
			}
		}

		m_prevTime += m_previousState->GetSpeed() * GTHTimes::Time_deltaTime;
		if (m_crtTime >= timeToBlend || m_prevTime >= m_previousState->GetDuration())
		{
			blendToNextState = false;
			//m_crtTime = timeToBlend;
		}

	}
	else
	{//ELSE CHECK IF NEXT FRAME SHOULD BEGIN BLENDING TO A NEXT STATE
		std::vector<AnimationTransition> &transitions = m_currentState->GetTransitions();
		for (uint16_t i = 0; i < transitions.size(); i++)
		{
			if (transitions[i].IsScheduled())
			{
				if (m_crtTime > transitions[i].GetPrevMandatoryTime())
				{
					m_currentState->Reset();
					timeToBlend = std::min(transitions[i].GetNextMaxFadeInTime(), m_currentState->GetDuration() - m_crtTime);
					m_previousState = m_currentState;
					m_currentState = (BaseAnimationState *)transitions[i].GetNextState();

					SoundManager &soundMgr = SoundManager::getInstance();
					soundMgr.PlaySound(m_currentState->GetSoundID());
					blendToNextState = true;
					m_prevTime = m_crtTime;
					m_crtTime = 0.f;

					zeroRootTransform = true;
					mStateIDTransitionFlag = m_currentState->GetStateID();
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void CharacterAnimator::FixedUpdate()
{
	mIsBat = mName == std::string("Tori");

	if (isFightStance) 
	{
		m_fightStanceGroundedState.SetDefaultNextState(&m_fightStanceGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
		m_defaultGroundedState.SetDefaultNextState(&m_fightStanceGroundedState);
	}
	else
	{
		m_fightStanceGroundedState.SetDefaultNextState(&m_defaultGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
		m_defaultGroundedState.SetDefaultNextState(&m_defaultGroundedState);///!!!!!!!!!!!!!!!!!!!!!!
	}
	zeroRootTransform = false;
	m_crtTime += m_currentState->GetSpeed() * GTHTimes::Time_deltaTime;
	
	
	ComputeDirectionVectors();

	wr_speed = glm::distance(glm::vec2(0, 0), glm::vec2(m_directionVecPtr->x, m_directionVecPtr->z));
	
	if (wr_speed != wr_speed)
		wr_speed = 0.f;
	HandleTimeoutTransitions();
	/////////////////////////////////////// CURRENT STATE ANIMATION ///////////////////////////////////////
	float blendSum = 0;
	{
		m_currentState->CalcBlendFactors(wr_speed, moveDirDotProduct, moveDirCrossProduct, enemyDotProduct, enemyCrossProduct);
		mBlendScheme = m_currentState->getBlendScheme();

		blendSum = mBlendScheme[0].first;
		mSkinnedMesh->SetAnimationAt(mBlendScheme[0].second->GetSkeletalAnim()->m_animaBoneStruct, m_crtTime);

		for (uint16_t i = 1; i < mBlendScheme.size(); i++)
		{
			blendSum += mBlendScheme[i].first;
			mSkinnedMesh->BlendWithAnimAt(mBlendScheme[i].second->GetSkeletalAnim()->m_animaBoneStruct, m_crtTime, mBlendScheme[i].first / blendSum);
		}
	}

	HandleTriggerTransitions();
	mSkinnedMesh->FinishPosing(zeroRootTransform);
}

//////////////////////////////////////////////////////////////////////////////////////////

void CharacterAnimator::Trigger(uint16_t action)
{
	//if (action != m_currentState->GetStateID())
	//{
		std::vector<AnimationTransition> &transitions = m_currentState->GetTransitions();
		for (uint i = 0; i < transitions.size(); i++)
		{
			if (transitions[i].GetNextState()->GetStateID() == action)
			{
#ifdef DEBUG_FIGHTMODE
				printf("action %d\n", action);
#endif
				transitions[i].Schedule();
				return;
			}
		}
	//}
}

glm::vec3 CharacterAnimator::_GetRootTranslation(std::vector<std::pair<float, CharacterAnimation*> > &blendScheme, float t)
{
	crtRootPositions.clear();

	glm::vec3 rootTrans = glm::vec3(0, 0, 0);

	float blendSum = 0.f;

	for (uint i = 0, sz = blendScheme.size(); i < sz; ++i)
	{
		glm::vec3 pos = CalcInterpolatedPosition(blendScheme[i].second->GetSkeletalAnim()->m_animaBoneStruct, t);
		crtRootPositions[blendScheme[i].second] = std::pair<glm::vec3, float>(pos, t);
		
		if (prevRootPositions.find(blendScheme[i].second) != prevRootPositions.end())
		{
			if(prevRootPositions[blendScheme[i].second].second < t)
				rootTrans += blendScheme[i].first * (pos - prevRootPositions[blendScheme[i].second].first);
		}
		blendSum += blendScheme[i].first;
	}
	prevRootPositions = crtRootPositions;
	
	return rootTrans;
}

glm::mat4 CharacterAnimator::_GetRootRotation(std::vector<std::pair<float, CharacterAnimation*> > &blendScheme, float t)
{
	crtRootPositions.clear();

	glm::mat4 rootRot = glm::mat4(1);

	float blendSum = 0.f;

	for (uint i = 0, sz = blendScheme.size(); i < sz; ++i)
	{
		glm::quat rot = CalcInterpolatedRotation(blendScheme[i].second->GetSkeletalAnim()->m_animaBoneStruct, t);
		
		crtRootRotations[blendScheme[i].second] = std::pair<glm::quat, float>(rot, t);

		if (prevRootRotations.find(blendScheme[i].second) != prevRootRotations.end())
		{
			if (prevRootRotations[blendScheme[i].second].second < t)
			{
				glm::quat deltaRot = glm::toQuat(glm::toMat4(rot) * glm::inverse(glm::toMat4(prevRootRotations[blendScheme[i].second].first)));
				//printf("%f \n", deltaRot.w);
				glm::quat weightedRot = glm::quat(deltaRot.w * blendScheme[i].first, deltaRot.x, deltaRot.y, deltaRot.z);
				
				rootRot = glm::toMat4(weightedRot) * rootRot;
			}
		}
		blendSum += blendScheme[i].first;
	}
	//printf("$%Blendsuum %f\n", blendSum);
	//printf("[%f] %f %f %f\n", rootRot.w, rootRot.x, rootRot.y, rootRot.z);
	prevRootRotations = crtRootRotations;

	return rootRot;
}

glm::mat4 CharacterAnimator::GetRootRotation()
{
	return GetRootRotation(1.f);
}

glm::mat4 CharacterAnimator::GetRootRotation(float deltaTrans)
{
	float addRot = m_currentState->GetRotation(deltaTrans);
	glm::mat4 additionalRotation = glm::rotate(glm::mat4(1), addRot, glm::vec3(0, 1, 0));
	
	//glm::mat4 rotation = _GetRootRotation(mBlendScheme, m_crtTime); //alex dinu
	//return additionalRotation;

	return additionalRotation;// *mAnimRotation;
}