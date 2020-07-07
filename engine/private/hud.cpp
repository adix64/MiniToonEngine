#include "HUD.h"


HUD::HUD(){}

void HUD::Init(){
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBarBase.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HealthBar"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar1.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP1"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar2.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP2"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar3.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP3"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar4.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP4"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar5.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP5"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar6.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP6"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/HealthBar7.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["HP7"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaLeft1.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam1"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaLeft2.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam2"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaLeft3.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam3"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaLeft4.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam4"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaRight1.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam5"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaRight2.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam6"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaRight3.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam7"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/StaminaRight4.png",
										glm::vec3(-0.9, .8, 0), glm::vec3(-0.6, 0.9, 0));
		sprites["stam8"] = S;
	}
	{
		Sprite *S = new Sprite("../assets/HUD/miniMap.png",
			glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));
		sprites["miniMap"] = S;
	}
	
}