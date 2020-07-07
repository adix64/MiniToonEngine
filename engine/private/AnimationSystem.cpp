#include <AnimationSystem.h>

AnimationSystem::AnimationSystem() {}
void AnimationSystem::FixedUpdate()
{
	//TODO: below would give more instruction cache coherence...
	//but this sets m_Yrotation to 0.0f in FightStanceLocomotionAnimState, for ??? reason
	//for (int i = 0, sz = mAnimators.size(); i < sz; i++)
		//mAnimators[i]->FixedUpdate();
}