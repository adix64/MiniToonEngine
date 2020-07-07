#pragma once
#include "../../../dependencies/rapidxml-1.13/rapidxml_utils.hpp";
#include <unordered_map>
#include <SDL_mixer.h>
#include <GlobalTime.h>
#define woosh1 0
#define woosh2 1
#define woosh3 2
#define woosh4 3
#define woosh5 4
#define woosh6 5
#define woosh7 6

#define hitSID 7


#define hit0SID 8
#define maxHITdSID 4
#define mynextSound (hit0SID + maxHITdSID + 1)

class SoundManager
{
private:
	SoundManager();
	// Stop the compiler from generating copy methods
	SoundManager(SoundManager const& copy);            // do NOT implement
	SoundManager& operator=(SoundManager const& copy); // do NOT implement
public:
	void Init();
	static SoundManager& getInstance()
	{
		static SoundManager instance;
		return instance;
	}
	void PlaySound(int soundID);
	void PlayMusic(int soundID, int loops = 0);
	void StopMusic();
private:
	std::unordered_map<int, Mix_Chunk*> sounds;
	std::unordered_map<int, Mix_Music*> music;
	Mix_Chunk *lastPlayed;
	Uint64 lastPlayedTimeStamp = -1;
public:
	bool muted = false;
};