#pragma once
#include <GL/glew.h>
#include <cstdio>
#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <vector>
using namespace std;
struct ShadowCascade
{
	struct AABB
	{
		float minX, maxX, minY, maxY, minZ, maxZ;
	};
	ShadowCascade()
	{
		boxesViewProj.resize(4);
		boxesView.resize(4);
	}

	void Update(glm::mat4 &projView, glm::vec3 &lightDir);
	std::vector<glm::mat4> boxesViewProj;
	std::vector<glm::mat4> boxesView;
};

class ShadowMapFBO
{
public:
	ShadowMapFBO();

	~ShadowMapFBO();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

	void BindForWriting();

	void BindForReading(GLenum TextureUnit);

//private:
	GLuint m_fbo;
	GLuint m_depth;
	GLuint m_shadowMap;
};

struct CascadedShadowMapFBO
{
	CascadedShadowMapFBO()
	{
		cascadeMaps.resize(4);
		for (int i = 0; i < 4; i++)
			cascadeMaps[i] = new ShadowMapFBO();
	}

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight)
	{
		for (int i = 0; i < 4; i++)
		{
			if (!cascadeMaps[i]->Init(WindowWidth, WindowHeight))
				return false;
		}
		return true;
	}
	void Update(glm::mat4 &projView, glm::vec3 &lightDir)
	{
		cascades.Update(projView, lightDir);
	}
	std::vector<ShadowMapFBO*> cascadeMaps;
	ShadowCascade cascades;
};