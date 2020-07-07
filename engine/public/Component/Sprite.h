#pragma once
#include "../../dependencies/glm/glm.hpp"
#include <iostream>
#include <Rendering/Shader.h>
//#include "Rendering/Texture.h"
class Sprite
{
public:
	Sprite();
	Sprite(glm::vec3 &lL, glm::vec3 &uR);
	Sprite(const char* filename, glm::vec3 &lL, glm::vec3 &uR);
	~Sprite();

	void SetCorners(glm::vec3 &lL, glm::vec3 &uR);

	void Render(Shader *shader);

	//void Render(Texture2D *tex);
	//void Render(unsigned int tex);
	//void Render(glm::vec3 &color);
	void LoadTexture(const char *filename);
	//Texture2D* GetTex() const { return m_texture; }
private:
	void Init();
public:
	glm::mat4 mvpMatrix;
private:
	unsigned int ibo, vao;
	unsigned int VBOs[2];


	glm::vec3 lowerLeft, upperRight;
	//Texture2D *m_texture;
public:
	unsigned int texid;
	bool visible = true;
};