#pragma once
#include <vector>
#include <Component/CharacterAnimation/AnimationTransition.h>
#include <Component/CharacterAnimation/CharacterAnimation.h>
//////////////////////////////////////////////////////////////////////////////
#define ATTACKING_LIMB_NONE 0
#define ATTACKING_LIMB_PUNCH1 (ATTACKING_LIMB_NONE + 1)
#define ATTACKING_LIMB_PUNCH2 (ATTACKING_LIMB_PUNCH1 + 1)
#define ATTACKING_LIMB_PUNCH3 (ATTACKING_LIMB_PUNCH2 + 1)
#define ATTACKING_LIMB_PUNCH4 (ATTACKING_LIMB_PUNCH3 + 1)
#define ATTACKING_LIMB_PUNCH5 (ATTACKING_LIMB_PUNCH4 + 1)

#define ATTACKING_LIMB_KICK1 (ATTACKING_LIMB_PUNCH5 + 1)
#define ATTACKING_LIMB_KICK2 (ATTACKING_LIMB_KICK1 + 1)
#define ATTACKING_LIMB_KICK3 (ATTACKING_LIMB_KICK2 + 1)
#define ATTACKING_LIMB_KICK4 (ATTACKING_LIMB_KICK3 + 1)
#define ATTACKING_LIMB_KICK5 (ATTACKING_LIMB_KICK4 + 1)

#define HIT_REACT_BACKOFF 0
#define HIT_REACT_LEFT (HIT_REACT_BACKOFF + 1)
#define HIT_REACT_RIGHT (HIT_REACT_LEFT + 1)

class BaseAnimationState //a vertex
{
public:
	BaseAnimationState() : m_playSpeed(1.0f), m_attackingLimb(ATTACKING_LIMB_NONE), mSoundID(-1){}
	BaseAnimationState(uint16_t stateID) : m_stateID(stateID), m_playSpeed(1.0f), m_attackingLimb(ATTACKING_LIMB_NONE){}
	virtual void Update()
	{
		
	}
	inline uint16_t GetStateID() const{ return m_stateID; }
	void SetDefaultNextState(BaseAnimationState *state)
	{
		m_defaultNextState = state;
	}
	char GetAttackingLimb()
	{
		return m_attackingLimb;
	}
	void SetAttackingLimb(char limb)
	{
		m_attackingLimb = limb;
	}
	BaseAnimationState* GetDefaultNextState()
	{
		return m_defaultNextState;
	}
	float GetSpeed() const { return m_playSpeed; }
	inline void SetSpeed(const float speed) {m_playSpeed = speed; }
	void Reset()
	{
		for (uint16_t i = 0; i < m_links.size(); i++)
		{
			m_links[i].Reset();
		}
	}

	virtual void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
		float enemyDotProduct, glm::vec3 &enemyCrossProduct){}
	virtual std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme()
	{
		std::vector<std::pair<float, CharacterAnimation*> > blendScheme;
		blendScheme.push_back(std::pair<float, CharacterAnimation*>(1.f, NULL));
		return blendScheme;
	}

	virtual float GetRotation(float deltaTrans){ return 0.0f; }
	void AddTransition(AnimationTransition at){ m_links.push_back(at); }
	std::vector<AnimationTransition>& GetTransitions(){ return m_links; }
	virtual float GetDuration()
	{ 
		return m_duration; 
	}

	virtual int GetTag() { return -1; }
	int GetSoundID() { return mSoundID; }
	void SetSoundID(int sid) { mSoundID = sid; }
//protected:
	int mSoundID;
	uint16_t m_stateID;
	char m_attackingLimb;
	std::vector<AnimationTransition> m_links;
	float m_playSpeed;
	float m_duration;
	bool mIsCycle = false;
	BaseAnimationState * m_defaultNextState;
};
