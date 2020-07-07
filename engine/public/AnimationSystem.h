#pragma once
#include <Component/CharacterAnimation/CharacterAnimator.h>

class Component;

class AnimationSystem
{
public:
	std::vector<CharacterAnimator*> mAnimators;
private:
	AnimationSystem();
	AnimationSystem(AnimationSystem const& copy);            // do NOT implement
	AnimationSystem& operator=(AnimationSystem const& copy); // do NOT implement
public:
	// Stop the compiler from generating copy methods
	static AnimationSystem& getInstance()
	{
		static AnimationSystem instance;
		return instance;
	}
	void FixedUpdate();
};