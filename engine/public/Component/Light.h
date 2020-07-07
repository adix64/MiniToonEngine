#pragma once
#include <GL\glew.h>
#include "../../../dependencies/glm/glm.hpp"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#define constant x
#define linear y
#define quadratic z
#include <Component/Camera.h>
typedef unsigned int uint;
struct Light {
	
	unsigned int id;
	
	glm::vec3 color;
	
	float power;
	float ambientIntensity;
	bool mShadowsOn;
	float luminance;
	void SetID(unsigned int id);	
};



class SpotLight : public Light {
public:
	glm::vec3 attenuation;
	Camera lightCam;
	glm::vec3 position, direction;
	float cutoff, blur_cutoff;
	float scaleFact;
	glm::mat4 mLightWorldMatrix;
	glm::mat4 scaleMat;
	bool visible = false;

	SpotLight(float power, float ambientIntensity, glm::vec3 position, glm::vec3 direction,
			  float cutoff, float blur_cutoff, glm::vec3 color, glm::vec3 attenuation);
};


class DirectionalLight : public Light {
public:
	glm::vec3 direction;
	DirectionalLight(float power, float ambientIntensity, glm::vec3 direction, glm::vec3 color, bool shadows = true);
};

class PointLight : public Light
{
public:
	glm::vec3 attenuation;
	std::vector<Camera> lightCams;
	float scaleFact;
	glm::vec3 position;
	glm::mat4 scaleMat;
	bool visible = false;
	PointLight(float power, float ambientIntensity, glm::vec3 position, glm::vec3 color, glm::vec3 attenuation, bool shadows);
};