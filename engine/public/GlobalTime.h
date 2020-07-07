#pragma once
#include <SDL.h>

namespace GTHTimes
{
extern Uint64 TIME_Now, TIME_renderNow;
extern Uint64 TIME_Last, TIME_renderLast;
extern float Time_deltaTime, TIME_renderTime;
extern bool TIME_pause;

extern float GLOBAL_orthoFar, GLOBAL_orthoNear;
void TIME_Init();
Uint64 GetTickCount();
void TIME_Tick();
void TIME_renderTick();
} //GTHTimes
