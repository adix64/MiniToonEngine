#include <Component/CharacterAnimation/BaseAnimationState.h>
#include <Component/CharacterAnimation/CharacterAnimation.h>
#include <ResourceManager.h>

//////////////////////////////////////////////////////////////////////////////

class FightStanceLocomotionAnimationState : public BaseAnimationState
{
public:
	FightStanceLocomotionAnimationState(){ m_Yrotation = 0; }
	FightStanceLocomotionAnimationState(uint16_t stateID, std::string & breatheFname,
									std::string &move_FwdL_file, std::string &move_FwdR_file,
									std::string &move_LeftFwd_file, std::string &move_RightFwd_file,
									std::string &move_Left_file, std::string &move_Right_file,
									std::string &move_LeftBack_file, std::string &move_RightBack_file,
									std::string &move_BackL_file, std::string &move_BackR_file,
									std::string &block_file,
									const glm::vec3 *fwdVecPtr, glm::vec3 *moveDirPtr, glm::vec3 *enemyDirPtr)
									: BaseAnimationState(stateID)
	{
		ResourceManager &resmgr = ResourceManager::getInstance();
		m_Yrotation = 0;
		m_idleAnimations.push_back(resmgr.GetCharacterAnimationTrack(breatheFname));
		
		m_move_FwdL_Anim = resmgr.GetCharacterAnimationTrack(move_FwdL_file);
		m_move_FwdR_Anim = resmgr.GetCharacterAnimationTrack(move_FwdR_file);
		
		m_move_LeftFwd_Anim = resmgr.GetCharacterAnimationTrack(move_LeftFwd_file);
		m_move_RightFwd_Anim = resmgr.GetCharacterAnimationTrack(move_RightFwd_file);

		m_move_Left_Anim = resmgr.GetCharacterAnimationTrack(move_Left_file);
		m_move_Right_Anim = resmgr.GetCharacterAnimationTrack(move_Right_file);

		m_move_LeftBack_Anim = resmgr.GetCharacterAnimationTrack(move_LeftBack_file);
		m_move_RightBack_Anim = resmgr.GetCharacterAnimationTrack(move_RightBack_file);

		m_move_BackL_Anim = resmgr.GetCharacterAnimationTrack(move_BackL_file);
		m_move_BackR_Anim = resmgr.GetCharacterAnimationTrack(move_BackR_file);


		m_blockAnim = resmgr.GetCharacterAnimationTrack(block_file);

		m_duration = m_move_FwdL_Anim->GetDuration();
		m_isBlocking = false;

		m_idleBlendFact = prev_m_idleBlendFact = 1.f;
		m_blockBlendFact  = m_move_FwdL_BlendFact = m_move_FwdR_BlendFact =
							m_move_LeftFwd_BlendFact = m_move_RightFwd_BlendFact = 
							m_move_Left_BlendFact = m_move_Right_BlendFact =
							m_move_LeftBack_BlendFact = m_move_RightBack_BlendFact =
							m_move_BackL_BlendFact = m_move_BackR_BlendFact = 0.0f;

		prev_m_blockBlendFact  = prev_m_move_FwdL_BlendFact = prev_m_move_FwdR_BlendFact =
							prev_m_move_LeftFwd_BlendFact = prev_m_move_RightFwd_BlendFact = 
							prev_m_move_Left_BlendFact = prev_m_move_Right_BlendFact =
							prev_m_move_LeftBack_BlendFact = prev_m_move_RightBack_BlendFact =
							prev_m_move_BackL_BlendFact = prev_m_move_BackR_BlendFact = 0.0f;
	}

	virtual float GetDuration();
	void SavePrevAndBlendWithIt();
	void SavePrev();
	void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
		float enemyDotProduct, glm::vec3 &enemyCrossProduct);
	
	void SetBlockMode(bool);
	float GetRotation(float deltaTrans);

	std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme();

//private:
	std::vector<CharacterAnimation *> m_idleAnimations; //breathe == idleAnimations[0] with a probability of 90% - 95%
	float m_idleBlendFact;

	const CharacterAnimation *m_move_FwdL_Anim;
	float m_move_FwdL_BlendFact;
	const CharacterAnimation *m_move_FwdR_Anim;
	float m_move_FwdR_BlendFact;

	const CharacterAnimation *m_move_LeftFwd_Anim;
	float m_move_LeftFwd_BlendFact;
	const CharacterAnimation *m_move_RightFwd_Anim;
	float m_move_RightFwd_BlendFact;

	const CharacterAnimation *m_move_Left_Anim;
	float m_move_Left_BlendFact;
	const CharacterAnimation *m_move_Right_Anim;
	float m_move_Right_BlendFact;

	const CharacterAnimation *m_move_LeftBack_Anim;
	float m_move_LeftBack_BlendFact;
	const CharacterAnimation *m_move_RightBack_Anim;
	float m_move_RightBack_BlendFact;

	const CharacterAnimation *m_move_BackL_Anim;
	float m_move_BackL_BlendFact;
	const CharacterAnimation *m_move_BackR_Anim;
	float m_move_BackR_BlendFact;

	//parameters
	const CharacterAnimation *m_blockAnim;
	float m_blockBlendFact;
	bool m_isBlocking;

	//glm::vec3 *m_characterFwdVecPtr;
	//glm::vec3 *m_moveDirPtr;
	//glm::vec3 *m_enemyDirPtr;

	float m_Yrotation;

	float m_timeout; //?????????????
	
	float prev_m_idleBlendFact, 
		prev_m_move_FwdL_BlendFact, prev_m_move_FwdR_BlendFact,
		prev_m_move_LeftFwd_BlendFact, prev_m_move_RightFwd_BlendFact,
		prev_m_move_Left_BlendFact, prev_m_move_Right_BlendFact,
		prev_m_move_LeftBack_BlendFact, prev_m_move_RightBack_BlendFact,
		prev_m_move_BackL_BlendFact, prev_m_move_BackR_BlendFact,
		prev_m_blockBlendFact;
};
