#pragma once
#include <SDL.h>
#include <GlobalTime.h>
#include <cstdio>

namespace GTHTimes
{
Uint64 TIME_Now, TIME_renderNow;
Uint64 TIME_Last, TIME_renderLast;
float Time_deltaTime, TIME_renderTime;

float GLOBAL_orthoFar;
float GLOBAL_orthoNear;

bool TIME_pause;
void TIME_Init()
{
	TIME_Now = TIME_renderNow = SDL_GetPerformanceCounter();
	TIME_Last = TIME_renderLast = 0;
	TIME_pause = false;
	GTHTimes::Time_deltaTime = TIME_renderTime = 0.0;
	GLOBAL_orthoFar = 34.f;
	GLOBAL_orthoNear = -300.f;
}
Uint64 GetTickCount()
{
	return SDL_GetTicks();
}

void TIME_Tick()
{
	if (TIME_pause)
	{
		GTHTimes::Time_deltaTime = 0.0f;
		return;
	}
	TIME_Last = TIME_Now;
	TIME_Now = SDL_GetPerformanceCounter();
	GTHTimes::Time_deltaTime = (float)(TIME_Now - TIME_Last) / (float)SDL_GetPerformanceFrequency();
	//printf("Time_Tick() %f\n", GTHTimes::Time_deltaTime);
}
void TIME_renderTick()
{
	TIME_renderLast = TIME_renderNow;
	TIME_renderNow = SDL_GetPerformanceCounter();
	GTHTimes::TIME_renderTime = (float)(TIME_renderNow - TIME_renderLast) / (float)SDL_GetPerformanceFrequency();
}
} //GTHTimes

