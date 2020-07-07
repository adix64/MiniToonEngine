#pragma once
#include "SkeletalAnimation.h"

class CharacterAnimation
{
public:
	CharacterAnimation(std::string &filename)
	{
		m_anim = new SkeletalAnimation(filename);
		m_duration_s = m_anim->m_maxTime;
	}

	inline void AddSweetSpot(uint16_t sweetSpotDefine, float time)
	{
		m_sweetSpots.push_back(std::pair<uint16_t, float>(sweetSpotDefine, time));
	}

	inline float GetDuration() const
	{
		return m_duration_s;
	}

	inline SkeletalAnimation* GetSkeletalAnim() { return this->m_anim; }
	//private:
	SkeletalAnimation *m_anim;
	float m_duration_s;
	std::vector<std::pair<uint16_t, float> > m_sweetSpots;
};
