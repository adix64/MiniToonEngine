#include <Rendering/GBuffer.h>
#include <Rendering/glError.h>
GBuffer::GBuffer()
{
	m_fbo = 0;
	m_depthTexture = 0;
	memset(m_textures, 0, GBUFFER_NUM_TEXTURES * sizeof(m_textures[0]));
	//memset(m_postProcessTextures, 0, 3 * sizeof(m_postProcessTextures[0]));
}

void GBuffer::Clear()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_textures[0] != 0) {
		glDeleteTextures(GBUFFER_NUM_TEXTURES, m_textures);
	}

	if (m_depthTexture != 0) {
		glDeleteTextures(1, &m_depthTexture);
	}
}

GBuffer::~GBuffer()
{
	Clear();
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	Clear();
	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	GLint internalFormats[GBUFFER_NUM_TEXTURES] =
	{
		GL_RGB32F,//GBUFFER_TEXTURE_TYPE_POSITION,
		GL_RGB8,//GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GL_RGB565,//GBUFFER_TEXTURE_TYPE_SPECULAR,
		GL_RGB32F,//GBUFFER_TEXTURE_TYPE_NORMAL,
		GL_RGB565,//GBUFFER_TEXTURE_TYPE_CHARACTERS,
		GL_RGB32F,//GBUFFER_TEXTURE_TYPE_DIFFUSELIGHTACC,
		GL_RGB32F,//GBUFFER_TEXTURE_TYPE_SPECLIGHTACC,
		//GL_RGB8//GBUFFER_TEXTURE_TYPE_FINAL,
	};

// Create the gbuffer textures
	glGenTextures(GBUFFER_NUM_TEXTURES, m_textures);
	for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
		
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormats[i], WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
	}

////post_process textures
//	glGenTextures(3, m_postProcessTextures);
//	for (unsigned int i = 0; i < 3; i++)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_postProcessTextures[i]);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST);
//		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES +  i, GL_TEXTURE_2D, m_postProcessTextures[i], 0);
//	}

//depth texture
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, WindowWidth, WindowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	return true;
}
void GBuffer::StartFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
void GBuffer::GBufferPassInit()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
							GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, 
							GL_COLOR_ATTACHMENT4};
	CheckOpenGLError();
	glDrawBuffers(5, DrawBuffers);
	CheckOpenGLError();
}

void GBuffer::LightPassInit()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6};
	CheckOpenGLError();
	glDrawBuffers(2, DrawBuffers);
	CheckOpenGLError();
}
//
//void GBuffer::FinalPassInit()
//{
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
//	CheckOpenGLError();
//	glDrawBuffer(GL_COLOR_ATTACHMENT8);
//	CheckOpenGLError();
//}


//
//void GBuffer::PostProcessPassInit()
//{
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
//	CheckOpenGLError();
//	//for (unsigned int i = 0; i < 3; i++)
//	//{
//	//	glActiveTexture(GL_TEXTURE1 + 5 + i);
//	//	glBindTexture(GL_TEXTURE_2D, m_postProcessTextures[i]);
//	//}
//	glReadBuffer(GL_COLOR_ATTACHMENT8);
//}