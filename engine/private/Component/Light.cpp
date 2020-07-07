#pragma once
#include <Component/Light.h>
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
typedef unsigned int uint;

struct CameraDirection
{
	GLenum CubemapFace;
	glm::vec3 Target;
	glm::vec3 Up;
};
#define NUM_OF_LAYERS 6

CameraDirection gCameraDirections[NUM_OF_LAYERS] =
{
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },//ok
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },//ok
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },//ok
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }//ok
};

void Light::SetID(unsigned int id)//virtual
{
	this->id = id;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
SpotLight::SpotLight(float power, float ambientIntensity, glm::vec3 position, glm::vec3 direction,
					  float cutoff, float blur_cutoff, glm::vec3 color, glm::vec3 attenuation)
{
	this->power = power;
	this->ambientIntensity = ambientIntensity;
	this->color = color;
	this->attenuation = attenuation;

	this->position = position;
	this->direction = direction;
	this->cutoff = cutoff;
	this->blur_cutoff = blur_cutoff;

	scaleFact = (-attenuation.linear + sqrt(attenuation.linear * attenuation.linear -
		4 * attenuation.quadratic * (attenuation.constant - 128))) / (2 * attenuation.quadratic);
	if (scaleFact != scaleFact)
		scaleFact = 500.f;
	scaleMat = glm::scale(glm::mat4(1), glm::vec3(scaleFact, scaleFact, scaleFact));

	glm::vec3 defaultDir = glm::vec3(0, 0, -1);
	glm::mat4 rot = glm::rotate(glm::mat4(1), acos(glm::dot(defaultDir, direction)), glm::normalize(glm::cross(defaultDir + glm::vec3(0,0.001,0), direction)));
	glm::mat4 trans = glm::translate(glm::mat4(1), position);
	float t = tan(acos(cutoff));
	mLightWorldMatrix = trans * rot * scaleMat * glm::scale(glm::mat4(1),glm::vec3(t,t,1));

	glm::vec3 up = (direction == glm::vec3(0, -1, 0)) ? 
					glm::normalize(glm::vec3(0.1, 1, 0)) : 
					glm::vec3(0, 1, 0);
	this->lightCam = Camera(position, position + direction, up);
	lightCam.SetSpotShadowMapProjection(cutoff);
	glm::vec3 luma = glm::vec3(0.299, 0.587, 0.114);
	luminance = glm::dot(color, luma);
}

///////////////////////////////////////////////////////////////////

DirectionalLight::DirectionalLight(float power, float ambientIntensity, glm::vec3 direction,
								   glm::vec3 color, bool shadows)
{
	this->power = power;
	this->ambientIntensity = ambientIntensity;
	this->direction = direction;
	this->color = color;
	glm::vec3 luma = glm::vec3(0.299, 0.587, 0.114);
	luminance = glm::dot(color, luma);
	mShadowsOn = shadows;
}

/////////////////////////////////////////////////////////////////////////////////////

PointLight::PointLight(float power, float ambientIntensity, glm::vec3 position,
					   glm::vec3 color, glm::vec3 attenuation, bool shadows)
{
	this->mShadowsOn = shadows;
	this->power = power;
	this->ambientIntensity = ambientIntensity;
	this->position = position;
	this->color = color;
	this->attenuation = attenuation;

	scaleFact = (-attenuation.linear + sqrt(attenuation.linear * attenuation.linear -
		4 * attenuation.quadratic * (attenuation.constant - 128))) / (2 * attenuation.quadratic);
	if (scaleFact != scaleFact)
		scaleFact = 500.f;
	scaleMat = glm::scale(glm::mat4(1), glm::vec3(scaleFact, scaleFact, scaleFact));

	for (int i = 0; i < 6; i++)
	{
		lightCams.push_back(Camera(position, position + gCameraDirections[i].Target, gCameraDirections[i].Up));
		lightCams[lightCams.size() - 1].SetOmniShadowMapProjection();
	}
	glm::vec3 luma = glm::vec3(0.299, 0.587, 0.114);
	luminance = glm::dot(color, luma);
}