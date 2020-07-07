#pragma once
#include <Component/CharacterAnimation/BaseAnimationState.h>
#include <Component/CharacterAnimation/CharacterAnimation.h>
#include <ResourceManager.h>
//////////////////////////////////////////////////////////////////////////////
class WalkRunAnimationState : public BaseAnimationState
{
public:
	WalkRunAnimationState()
	{
		m_Yrotation = 0;
	}
	WalkRunAnimationState(uint16_t stateID, std::string & breatheFname,
		std::string &walkFwdFname, std::string &walkLeftFname, std::string &walkRightFname,
		std::string &runFwdFname, std::string &runLeftFname, std::string &runRightFname,
		const glm::vec3 *fwdVecPtr, glm::vec3 *moveDirPtr)
		: BaseAnimationState(stateID)//, m_characterFwdVecPtr(fwdVecPtr), m_directionVecPtr(moveDirPtr)
	{
		ResourceManager &resmgr = ResourceManager::getInstance();
		m_Yrotation = 0;
		m_idleAnimations.push_back(resmgr.GetCharacterAnimationTrack(breatheFname));
		m_walkFwdAnim = resmgr.GetCharacterAnimationTrack(walkFwdFname);
		m_walkLeftAnim = resmgr.GetCharacterAnimationTrack(walkLeftFname);
		m_walkRightAnim = resmgr.GetCharacterAnimationTrack(walkRightFname);
		m_runFwdAnim = resmgr.GetCharacterAnimationTrack(runFwdFname);
		m_runLeftAnim = resmgr.GetCharacterAnimationTrack(runLeftFname);
		m_runRightAnim = resmgr.GetCharacterAnimationTrack(runRightFname);
		m_duration = m_runFwdAnim->GetDuration();

		m_idleBlendFact = prev_m_idleBlendFact = 1.f;

		m_walkLeftBlendFact = m_walkRightBlendFact = m_walkFwdBlendFact = 
			m_runLeftBlendFact = m_runRightBlendFact = m_runFwdBlendFact = 0.0f;

		prev_m_runFwdBlendFact = prev_m_runLeftBlendFact = prev_m_runRightBlendFact = 
		prev_m_walkFwdBlendFact = prev_m_walkLeftBlendFact = prev_m_walkRightBlendFact = 0.0f;
	}

	virtual float GetDuration();
	void SavePrevAndBlendWithIt();
	void SavePrev();
	void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
		float enemyDotProduct, glm::vec3 &enemyCrossProduct);
	float GetRotation(float deltaTrans);
	std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme();

//private:
	std::vector<CharacterAnimation *> m_idleAnimations; //breathe == idleAnimations[0] with a probability of 90% - 95%
	float m_idleBlendFact, prev_m_idleBlendFact;

	const CharacterAnimation *m_walkFwdAnim, *m_runFwdAnim;
	float m_walkFwdBlendFact, m_runFwdBlendFact, prev_m_walkFwdBlendFact, prev_m_runFwdBlendFact;

	const CharacterAnimation *m_walkLeftAnim, *m_runLeftAnim;
	float m_walkLeftBlendFact, m_runLeftBlendFact, prev_m_walkLeftBlendFact, prev_m_runLeftBlendFact;

	const CharacterAnimation *m_walkRightAnim, *m_runRightAnim;
	float m_walkRightBlendFact, m_runRightBlendFact, prev_m_walkRightBlendFact, prev_m_runRightBlendFact;
//parameters
	//glm::vec3 *m_characterFwdVecPtr;
	//glm::vec3 *m_directionVecPtr;
	float m_Yrotation;
};
