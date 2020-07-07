#include <stdio.h>
#include <Rendering/ShadowCubeMapFBO.h>
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShadowCubeMapFBO::Clear()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_shadowMap != 0) {
		glDeleteTextures(1, &m_shadowMap);
	}

	if (m_depth != 0) {
		glDeleteTextures(1, &m_depth);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShadowCubeMapFBO::ShadowCubeMapFBO()
{
    m_fbo = 0;
    m_shadowMap = 0;	
    m_depth = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShadowCubeMapFBO::~ShadowCubeMapFBO()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShadowCubeMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	Clear();
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_depth);
    glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create the cube map
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0 ; i < 6 ; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, NULL);///?????????????????B
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
       
    // Disable reads from the color buffer
    glReadBuffer(GL_NONE);
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
		return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}


void ShadowCubeMapFBO::BindForWriting(GLenum CubeFace)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CubeFace, m_shadowMap, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void ShadowCubeMapFBO::BindForReading(GLenum TextureUnit, int uniform_location)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
	glUniform1i(uniform_location, TextureUnit - GL_TEXTURE0);
}
