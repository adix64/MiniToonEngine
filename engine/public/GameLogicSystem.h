#pragma once
#include <vector>

class Component;

class GameLogicSystem
{
public:
	std::vector<Component*> mScripts;
private:
	GameLogicSystem();
	GameLogicSystem(GameLogicSystem const& copy);            // do NOT implement
	GameLogicSystem& operator=(GameLogicSystem const& copy); // do NOT implement
public:
	// Stop the compiler from generating copy methods
	static GameLogicSystem& getInstance()
	{
		static GameLogicSystem instance;
		return instance;
	}
	void FixedUpdate();
};