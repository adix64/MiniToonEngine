#pragma once

#include <Rendering/Vertex.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtx/quaternion.hpp"
#include "../../../dependencies/glm/gtx/dual_quaternion.hpp"
#include "../../../dependencies/glm/gtc/type_ptr.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
#include "../assimp-3.3.1/include/assimp/Importer.hpp"
#include "../assimp-3.3.1/include/assimp/scene.h"
#include "../assimp-3.3.1/include/assimp/postprocess.h"
#include <Rendering/Shader.h>

typedef unsigned int uint;

struct Mesh {
protected:
	glm::mat4 modelMatrix;
public:
	bool updated = false;		
	bool castsShadow = true;
	bool isVisible = true;
	bool solid_color = false;
	std::string name;
	std::vector<glm::vec3> objSpaceVerts; //as read from the .obj file
	std::vector<glm::vec3> worldSpaceVerts;
	glm::vec3 position;

	std::vector<GLuint> indicesCPU;
	unsigned int vbo, ibo, vao, count;
	GLfloat r, g, b;
		

	Mesh();

	Mesh(unsigned int vbo, unsigned ibo, unsigned vao, unsigned count, std::vector<glm::vec3> &worldSpaceVerts, std::vector<uint> &indicesCPU);

	void SetModelMatrix(glm::mat4 &modelMat);

	glm::mat4 GetModelMatrix();
		
	virtual void Update();

	void setMeshName(const char* n);

	~Mesh();

	virtual void Render(Shader * shader, bool color = true);

	void init(std::vector<VertexFormat>vertices, std::vector<GLuint> indices);

	void updateVertices();

	//mesh transformation functions
	void setPosition(glm::vec3 newPosition);

	void translateWorldX(GLfloat distance);

	void translateWorldY(GLfloat distance);

	void translateWorldZ(GLfloat distance);

	void scaleLocalX(GLfloat scale_factor);

	void scaleLocalY(GLfloat scale_factor);

	void scaleLocalZ(GLfloat scale_factor);

	void rotateLocalX(GLfloat angle);

	void rotateLocalY(GLfloat angle);

	void rotateLocalZ(GLfloat angle);

	void setColor(GLfloat red, GLfloat green, GLfloat blue);

	//GILBERT-JOHNSON-KEERTHI (GJK) collision detection algorithm, returns true if this mesh collides "test" mesh
	//Function tries to encapsulate the origin within a simplex built by two support points of this mesh and "test" in a search direction
	//updated iteratively, the support is the Minkowski sum of the furthest two points of this mesh and the reflection of "test"
	bool collidesWith(Mesh *test);
};
