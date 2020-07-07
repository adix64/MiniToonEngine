#include <Rendering/ShadowMapFBO.h>
#include <GlobalTime.h>
#include <stdio.h>
void ShadowCascade::Update(glm::mat4 &projView, glm::vec3 &lightDir)
{
	glm::mat4 invProjView = glm::inverse(projView);

	float pp = -1.f;
	float coef = -(lightDir.x * 3.f + lightDir.y * 2.f) / lightDir.z;
	glm::vec3 upVec = glm::normalize(glm::vec3(3, 2, coef));
	float deltas[4] = { 1.8, 0.11, 0.07, 0.02 };
	AABB boxes[4];
	for (int i = 0; i < 4; i++)
	{//calcule subfrustum's AABB
		//printf("_________boxNumber: %d\n", i);

		glm::vec4 botLeftNear = invProjView * glm::vec4(-1, -1, -1, 1);
		glm::vec4 botRightNear = invProjView * glm::vec4(1, -1, -1, 1);
		glm::vec4 topRightNear = invProjView * glm::vec4(1, 1, -1, 1);
		glm::vec4 topLeftNear = invProjView * glm::vec4(-1, 1, -1, 1);
		//glm::vec4 botLeftNear = invProjView * glm::vec4(-1, -1, pp, 1);
		//glm::vec4 botRightNear = invProjView * glm::vec4(1, -1, pp, 1);
		//glm::vec4 topRightNear = invProjView * glm::vec4(1, 1, pp, 1);
		//glm::vec4 topLeftNear = invProjView * glm::vec4(-1, 1, pp, 1);

		glm::vec4 botLeftFar = invProjView * glm::vec4(-1, -1, pp + deltas[i], 1);
		glm::vec4 botRightFar = invProjView * glm::vec4(1, -1, pp + deltas[i], 1);
		glm::vec4 topRightFar = invProjView * glm::vec4(1, 1, pp + deltas[i], 1);
		glm::vec4 topLeftFar = invProjView * glm::vec4(-1, 1, pp + deltas[i], 1);

		botLeftNear /= botLeftNear.w;
		botRightNear /= botRightNear.w;
		topLeftNear /= topLeftNear.w;
		topRightNear /= topRightNear.w;

		botLeftFar /= botLeftFar.w;
		botRightFar /= botRightFar.w;
		topLeftFar /= topLeftFar.w;
		topRightFar /= topRightFar.w;
		//find AABB in world space
		float minX = min(botLeftNear.x, min(botRightNear.x, min(topRightNear.x, min(topLeftNear.x, min(
			botLeftFar.x, min(botRightFar.x, min(topRightFar.x, topLeftFar.x)))))));
		float maxX = max(botLeftNear.x, max(botRightNear.x, max(topRightNear.x, max(topLeftNear.x, max(
			botLeftFar.x, max(botRightFar.x, max(topRightFar.x, topLeftFar.x)))))));

		float minY = min(botLeftNear.y, min(botRightNear.y, min(topRightNear.y, min(topLeftNear.y, min(
			botLeftFar.y, min(botRightFar.y, min(topRightFar.y, topLeftFar.y)))))));
		float maxY = max(botLeftNear.y, max(botRightNear.y, max(topRightNear.y, max(topLeftNear.y, max(
			botLeftFar.y, max(botRightFar.y, max(topRightFar.y, topLeftFar.y)))))));

		float minZ = min(botLeftNear.z, min(botRightNear.z, min(topRightNear.z, min(topLeftNear.z, min(
			botLeftFar.z, min(botRightFar.z, min(topRightFar.z, topLeftFar.z)))))));
		float maxZ = max(botLeftNear.z, max(botRightNear.z, max(topRightNear.z, max(topLeftNear.z, max(
			botLeftFar.z, max(botRightFar.z, max(topRightFar.z, topLeftFar.z)))))));

		if (i == 3)
		{
			minX = max(-5000.f, minX);
			maxX = min(5000.f, maxX);

			minY = max(-5000.f, minY);
			maxY = min(5000.f, maxY);

			minZ = max(-5000.f, minZ);
			maxZ = min(5000.f, maxZ);
		}

		//minX -= 20.f;
		//minY -= 20.f;
		//minZ -= 20.f;
		//maxX += 20.f;
		//maxY += 20.f;
		//maxZ += 20.f;
		//maxY = max(60.f, maxY);
		
		//transform wsAABB's verts to camera space
	
		glm::vec3 boxCenter = glm::vec3((minX + maxX) * 0.5f,
									    (minY + maxY) * 0.5f,
										(minZ + maxZ) * 0.5f);
		//boxCenter = glm::vec3(0, 0, 0);
		glm::mat4 lightView = glm::lookAt(boxCenter, boxCenter + lightDir, upVec);

		botLeftNear = lightView * glm::vec4(minX, minY, minZ, 1);
		botRightNear = lightView * glm::vec4(maxX, minY, minZ, 1);
		topRightNear = lightView * glm::vec4(maxX, maxY, minZ, 1);
		topLeftNear = lightView * glm::vec4(minX, maxY, minZ, 1);

		botLeftFar = lightView * glm::vec4(minX, minY, maxZ, 1);
		botRightFar = lightView * glm::vec4(maxX, minY, maxZ, 1);
		topRightFar = lightView * glm::vec4(maxX, maxY, maxZ, 1);
		topLeftFar = lightView * glm::vec4(minX, maxY, maxZ, 1);

		// get csAABB

		boxes[i].minX = min(botLeftNear.x, min(botRightNear.x, min(topRightNear.x, min(topLeftNear.x,
						min(botLeftFar.x,  min(botRightFar.x,  min(topRightFar.x,      topLeftFar.x)))))));
		boxes[i].maxX = max(botLeftNear.x, max(botRightNear.x, max(topRightNear.x, max(topLeftNear.x,
						max(botLeftFar.x,  max(botRightFar.x,  max(topRightFar.x,      topLeftFar.x)))))));

		boxes[i].minY = min(botLeftNear.y, min(botRightNear.y, min(topRightNear.y, min(topLeftNear.y,
						min(botLeftFar.y,  min(botRightFar.y,  min(topRightFar.y,	   topLeftFar.y)))))));
		boxes[i].maxY = max(botLeftNear.y, max(botRightNear.y, max(topRightNear.y, max(topLeftNear.y, 
						max(botLeftFar.y,  max(botRightFar.y,  max(topRightFar.y,	   topLeftFar.y)))))));

		boxes[i].minZ = min(botLeftNear.z, min(botRightNear.z, min(topRightNear.z, min(topLeftNear.z,
						min(botLeftFar.z,  min(botRightFar.z,  min(topRightFar.z,	   topLeftFar.z)))))));
		boxes[i].maxZ = max(botLeftNear.z, max(botRightNear.z, max(topRightNear.z, max(topLeftNear.z,
						max(botLeftFar.z,  max(botRightFar.z,  max(topRightFar.z,      topLeftFar.z)))))));

		pp += deltas[i];

		//
		//
		////float left = -30, right = 30, bottom = -30, top = 30, near = -30, far = 30;
		//printf("minX %.2f, maxX %.2f\n", boxes[i].minX, boxes[i].maxX);
		//printf("minY %.2f, maxY %.2f\n", boxes[i].minY, boxes[i].maxY);
		//printf("minZ %.2f, maxZ %.2f\n\n", boxes[i].minZ, boxes[i].maxZ);
		//float heX = (right - left) * 0.5,
		//	heY = (top - bottom) * 0.5,
		//	heZ = (far - near) * 0.5;



		boxesViewProj[i] = //glm::scale(glm::mat4(1), glm::vec3(1/boxes[i].maxX,1/boxes[i].maxY,1/boxes[i].maxZ)) *
						   //glm::ortho(left,right,bottom,top,near,far) * 
			glm::ortho(boxes[i].minX, boxes[i].maxX, 
					   boxes[i].minY, boxes[i].maxY, 
					   boxes[i].minZ + GTHTimes::GLOBAL_orthoNear, boxes[i].maxZ + GTHTimes::GLOBAL_orthoFar)
			* lightView;
		boxesView[i] = lightView;
	}

}

ShadowMapFBO::ShadowMapFBO()
{
	m_fbo = 0;
	m_depth = 0;
	m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_shadowMap != 0) {
		glDeleteTextures(1, &m_shadowMap);
	}
	if (m_depth!= 0) {
		glDeleteTextures(1, &m_depth);
	}
}

bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	// Create the FBO
	glGenFramebuffers(1, &m_fbo);

	// Create the depth buffer
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Create the depth buffer
	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_shadowMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
	// Disable writes to the color buffer
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}


void ShadowMapFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}