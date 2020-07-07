#pragma once
#include <GL/glew.h>
#include <cstdio>
#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <vector>
using namespace std;

class ColorFBO
{
public:
	ColorFBO();

	~ColorFBO();

	bool Init(GLint internalFormat, unsigned int WindowWidth, unsigned int WindowHeight, bool depthTexture = false, int givenDepthTex = -1, int filtering = GL_NEAREST);
	void Clear();
	void BindForWriting();

	void BindForReading(GLenum TextureUnit);

//private:
	GLuint m_fbo;
	GLuint m_texture;
	GLuint m_depth;
	bool hasExternalDepth = false;
};