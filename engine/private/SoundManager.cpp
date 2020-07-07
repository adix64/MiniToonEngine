#include <SoundManager.h>

#include <cstdio>
SoundManager::SoundManager()
{
	Init();
}

void SoundManager::Init()
{

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		printf("Error: Mix_OpenAudio()\n");
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh1.wav");
		sounds[woosh1] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh2.wav");
		sounds[woosh2] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh3.wav");
		sounds[woosh3] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh4.wav");
		sounds[woosh4] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh5.wav");
		sounds[woosh5] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh6.wav");
		sounds[woosh6] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Woosh7.wav");
		sounds[woosh7] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Hit1.wav");
		sounds[hit0SID] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Hit2.wav");
		sounds[hit0SID + 1] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Hit3.wav");
		if (!chnk) {
			printf("Mix_LoadWAV: %s\n", Mix_GetError());
			// handle error
		}
		sounds[hit0SID + 2] = chnk;
	}
	{
		Mix_Chunk* chnk = Mix_LoadWAV("../SFX/Hit4.wav");
		sounds[hit0SID + 3] = chnk;
	}
	lastPlayed = NULL;
}

void SoundManager::PlayMusic(int soundID, int loops)
{
	if (music.find(soundID) != music.end())
	{
		Mix_Music *mus = music[soundID];
		Mix_PlayMusic(mus, loops);
	}
}

void SoundManager::StopMusic()
{
	Mix_HaltMusic();
}
void SoundManager::PlaySound(int soundID)
{
	if (muted)return;
	if (soundID == hitSID)
	{
		soundID = hit0SID + (rand() % maxHITdSID);
	}
	if (sounds.find(soundID) != sounds.end())
	{
		Mix_Chunk *snd = sounds[soundID];
		Uint64 tickCnt = GTHTimes::GetTickCount();
		if (tickCnt - lastPlayedTimeStamp > 300 || snd != lastPlayed)
		{
			Mix_PlayChannel(0, sounds[soundID], 0);
			lastPlayed = snd;
			lastPlayedTimeStamp = tickCnt;
			printf("Playing sound %d\n", soundID);
		}
	}
}