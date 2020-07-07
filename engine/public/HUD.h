#pragma once
#include <Component/Sprite.h>

class HUD
{
public:
	std::unordered_map<std::string, Sprite *> sprites;
	HUD();
	void Init();
};
