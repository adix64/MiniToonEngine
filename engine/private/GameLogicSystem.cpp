#include <GameLogicSystem.h>
#include <Component/Component.h>

GameLogicSystem::GameLogicSystem() {}

void GameLogicSystem::FixedUpdate()
{
	for (int i = 0, sz = mScripts.size(); i < sz; i++)
	{
		mScripts[i]->FixedUpdate();
	}
}