#pragma once
#include <GL/glew.h>
#include <climits>
#include "Rendering/Texture.h"
#include "../../../dependencies/glm/glm.hpp"
#define COLOR_AND_ILLUMINATION 0
#define COLOR_ONLY 1
#define SILHOUETTE_ONLY 2
struct Material
{
	int diffuseMap = -1, normalMap = -1, specularMap = -1, emissiveMap = -1;
	glm::vec3 color;
	float emissiveFactor;
	Material() { color = glm::vec3(0.5, 0.5, 0.7); emissiveFactor = 0; }
};