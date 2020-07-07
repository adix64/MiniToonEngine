#pragma once
#include <Component/CharacterAnimation/BaseAnimationState.h>
#include <Component/CharacterAnimation/CharacterAnimation.h>
//////////////////////////////////////////////////////////////////////////////
class AttackAnimationState : public BaseAnimationState
{
public:
	AttackAnimationState();
	AttackAnimationState(uint16_t stateID, CharacterAnimation * _anim);
	AttackAnimationState(uint16_t stateID, int attackFileTag, std::string & chname);

	float GetRotation(float);
	void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct, 
														float enemyDotProduct, glm::vec3 &enemyCrossProduct);
	virtual std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme();
	int GetTag() { return stateTag; }
	std::string m_animName;
	CharacterAnimation *m_animation;
	float m_Yrotation;
	int stateTag;
	float sweetSpot = 0.f;
};

class HitReactAnimState : public BaseAnimationState
{
public:
	HitReactAnimState();
	HitReactAnimState(uint16_t stateID, std::vector<std::string> &animaFilenames);

	float GetRotation(float);

	void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
		float enemyDotProduct, glm::vec3 &enemyCrossProduct);

	virtual std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme();

	int GetTag() { return stateTag; }

	void SetTag(int tag);

	std::string m_animName;//????????????
	std::vector<CharacterAnimation *> hitAnims;
	float m_Yrotation;
	int stateTag = HIT_REACT_BACKOFF;
};


class TakeAmortizedHitAnimState : public BaseAnimationState
{
public:
	TakeAmortizedHitAnimState();
	TakeAmortizedHitAnimState(uint16_t stateID, CharacterAnimation *_takeHit, CharacterAnimation *_block);

	float GetRotation(float);
	
	void CalcBlendFactors(float wr_speed, float moveDirDotProduct, glm::vec3 &moveDirCrossProduct,
		float enemyDotProduct, glm::vec3 &enemyCrossProduct);
	
	virtual std::vector<std::pair<float, CharacterAnimation*> > getBlendScheme();
	
	int GetTag() { return stateTag; }
	
	std::string m_animName;
	CharacterAnimation *takeHitAnim, *blockAnim;
	float m_Yrotation;
	int stateTag;
};






