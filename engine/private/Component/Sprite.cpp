#pragma once
#include <Component/Sprite.h>
#include <Rendering/Texture.h>

Sprite::Sprite()
{
	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////////

Sprite::Sprite(glm::vec3 &lL, glm::vec3 &uR)
{
	Init();
	SetCorners(lL, uR);
}

//////////////////////////////////////////////////////////////////////////////////////////////

Sprite::Sprite(const char* filename, glm::vec3 &lL, glm::vec3 &uR)
{
	Init();
	SetCorners(lL, uR);
	LoadTexture(filename);
}

Sprite::~Sprite()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, VBOs);
	glDeleteBuffers(1, &ibo);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::SetCorners(glm::vec3 &lL, glm::vec3 &uR)
{
	glBindVertexArray(vao);
	lowerLeft = lL; upperRight = uR;

	std::vector<glm::vec3> verts; std::vector<glm::vec2> texCoords;
	verts.push_back(glm::vec3(lL.x, lL.y, 0)); texCoords.push_back(glm::vec2(0, 0));
	verts.push_back(glm::vec3(uR.x, lL.y, 0)); texCoords.push_back(glm::vec2(1, 0));
	verts.push_back(glm::vec3(uR.x, uR.y, 0)); texCoords.push_back(glm::vec2(1, 1));
	verts.push_back(glm::vec3(lL.x, uR.y, 0)); texCoords.push_back(glm::vec2(0, 1));

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), &verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//POSITIONS

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), &texCoords[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);//TexCoords
	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Render(Shader *shader)
{
	//glViewport(0, 0, *m_width, *m_height);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLboolean depthEnabled;
//	GLboolean blendingEnabled;
//	glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
//	glGetBooleanv(GL_BLEND, &blendingEnabled);



	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->Use();
	glBindVertexArray(vao);

	const GLint POS_LOCATION = shader->GetAttributeLocation("Position");
	const GLint TC_LOCATION = shader->GetAttributeLocation("TexCoordinates");

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glEnableVertexAttribArray(POS_LOCATION);
	glVertexAttribPointer(POS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glEnableVertexAttribArray(TC_LOCATION);
	glVertexAttribPointer(TC_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texid);
	//glUniform1i(shader->GetUniformLocation("solidColor"), 0);
	glUniform1i(shader->GetUniformLocation("diffuseMap"), 1);
	glUniformMatrix4fv(shader->GetUniformLocation("MVP"), 1, GL_FALSE, &mvpMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	/*if (depthEnabled != GL_FALSE)
		glEnable(GL_DEPTH_TEST);
	if (blendingEnabled == GL_FALSE)
		glDisable(GL_BLEND);*/
}

//////////////////////////////////////////////////////////////////////////////////////////////

//void Sprite::Render(Texture2D *tex)
//{
//	glViewport(0, 0, *m_width, *m_height);
//	////glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLboolean depthEnabled;
//	GLboolean blendingEnabled;
//	glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
//	glGetBooleanv(GL_BLEND, &blendingEnabled);
//	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	m_shader->Use();
//	glBindVertexArray(vao);
//	glActiveTexture(GL_TEXTURE0 + 1);
//	glBindTexture(GL_TEXTURE_2D, tex->GetTextureID());
//	glUniform1i(m_shader->GetUniformLocation(std::string("solidColor")), 0);
//	glUniform1i(m_shader->GetUniformLocation(std::string("fullscreenTex")), 1);
//	glUniform1i(m_shader->GetUniformLocation(std::string("width")), *m_width);
//	glUniform1i(m_shader->GetUniformLocation(std::string("height")), *m_height);

//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
//	glBindVertexArray(0);
//	if (depthEnabled != GL_FALSE)
//		glEnable(GL_DEPTH_TEST);
//	if (blendingEnabled == GL_FALSE)
//		glDisable(GL_BLEND);
//}

//////////////////////////////////////////////////////////////////////////////////////////////

//void Sprite::Render(unsigned int tex)
//{
//	glViewport(0, 0, *m_width, *m_height);
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLboolean depthEnabled;
//	GLboolean blendingEnabled;
//	glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
//	glGetBooleanv(GL_BLEND, &blendingEnabled);
//	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	m_shader->Use();
//	glBindVertexArray(vao);
//	glActiveTexture(GL_TEXTURE0 + 1);
//	glBindTexture(GL_TEXTURE_2D, tex);
//	glUniform1i(m_shader->GetUniformLocation(("solidColor")), 0);
//	glUniform1i(m_shader->GetUniformLocation(("fullscreenTex")), 1);
//	glUniform1i(m_shader->GetUniformLocation(("width")), *m_width);
//	glUniform1i(m_shader->GetUniformLocation(("height")), *m_height);

//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
//	glBindVertexArray(0);
//	if (depthEnabled != GL_FALSE)
//		glEnable(GL_DEPTH_TEST);
//	if (blendingEnabled == GL_FALSE)
//		glDisable(GL_BLEND);
//}

//////////////////////////////////////////////////////////////////////////////////////////////

//void Sprite::Render(glm::vec3 &color)
//{
//	glViewport(0, 0, *m_width, *m_height);
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLboolean depthEnabled;

//	glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
//	glDisable(GL_DEPTH_TEST);

//	m_shader->Use();
//	glBindVertexArray(vao);
//	glUniform1i(m_shader->GetUniformLocation(("solidColor")), 1);
//	glUniform3f(m_shader->GetUniformLocation(("inColor")), color.x,color.y,color.z);
//	glUniform1i(m_shader->GetUniformLocation(("width")), *m_width);
//	glUniform1i(m_shader->GetUniformLocation(("height")), *m_height);

//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
//	glBindVertexArray(0);
//	if (depthEnabled != GL_FALSE)
//		glEnable(GL_DEPTH_TEST);
//}

//////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::LoadTexture(const char *filename)
{
	texid = Texture::png_texture_load(filename, NULL, NULL, GL_RGBA8);
	//m_texture = new Texture2D();
	//m_texture->Load2D(filename.c_str(), GL_REPEAT);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Texture2D* Sprite::GetTex() const { return m_texture; }

//////////////////////////////////////////////////////////////////////////////////////////////


void Sprite::Init()
{
	std::vector<unsigned> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, VBOs);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	mvpMatrix = glm::mat4(1);
}
