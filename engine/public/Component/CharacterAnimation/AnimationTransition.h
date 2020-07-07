#pragma once
class BaseAnimationState;

class AnimationTransition //an edge
{
public:
	AnimationTransition(BaseAnimationState *nextState, float prevMandatoryTime, float nextMaxFadeInTime = 0.0f) :
		m_nextState(nextState), m_prevMandatoryTime(prevMandatoryTime), m_nextMaxFadeInTime(nextMaxFadeInTime)
	{
		Reset();
	}
	//AnimationTransition() : m_nextState(NULL), m_prevMandatoryTime(0), m_nextMaxFadeInTime(0) {}
	inline float GetPrevMandatoryTime() const { return m_prevMandatoryTime; }
	inline float GetNextMaxFadeInTime() const { return m_nextMaxFadeInTime; }

	

	inline void Schedule() { m_scheduled = true; }
	inline void Unschedule() { m_scheduled = false; }
	inline bool IsScheduled() { return m_scheduled; }

	inline void Reset() { Unschedule(); }

	const BaseAnimationState* GetNextState() { return m_nextState; }

	AnimationTransition operator=(const AnimationTransition& rhs)
	{
		AnimationTransition at((BaseAnimationState *)rhs.m_nextState, rhs.m_prevMandatoryTime, rhs.m_nextMaxFadeInTime);
		at.m_scheduled= rhs.m_scheduled;
		return at;
	}
//private:
	const BaseAnimationState *m_nextState;
	const float m_prevMandatoryTime;
	const float m_nextMaxFadeInTime;
	bool m_scheduled;
};

