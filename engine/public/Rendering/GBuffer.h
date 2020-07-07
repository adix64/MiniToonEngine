#pragma once
#ifndef GBUFFER_H
#define	GBUFFER_H

#include <GL/glew.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class GBuffer
{
public:

	enum GBUFFER_TEXTURE_TYPE
	{
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_SPECULAR,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_CHARACTERS,
		GBUFFER_TEXTURE_TYPE_DIFFUSELIGHTACC,
		GBUFFER_TEXTURE_TYPE_SPECLIGHTACC,
		//GBUFFER_TEXTURE_TYPE_FINAL,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();

	~GBuffer();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
	void Clear();
	void StartFrame();
	void GBufferPassInit();
	void LightPassInit();
	//void FinalPassInit();
	//void PostProcessPassInit();

	inline GLuint GetPositionsTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_POSITION]; }
	inline GLuint GetDiffuseTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_DIFFUSE]; }
	inline GLuint GetSpecularTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_SPECULAR]; }
	inline GLuint GetNormalsTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_NORMAL]; }
	inline GLuint GetMaterialInfoTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_CHARACTERS]; }
	inline GLuint GetDiffuseLightAccumulationTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_DIFFUSELIGHTACC]; }
	inline GLuint GetSpecularLightAccumulationTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_SPECLIGHTACC]; }
	inline GLuint GetDepthTexture() { return m_depthTexture; }
	//inline GLuint GetFinalTexture() { return m_textures[GBUFFER_TEXTURE_TYPE_FINAL]; }
//private:
	GLuint m_fbo;
	GLuint m_textures[GBUFFER_NUM_TEXTURES];
	GLuint m_depthTexture;
};

#endif	/* SHADOWMAPFBO_H */

