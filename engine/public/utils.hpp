/*#ifdef __UBG_UTILS
#define __UBG_UTILS
#pragma once
#include <vector>
#include <map>
#include "Rendering/glew.h"
#include <gl/GL.h>
#include "../../../dependencies/glm/glm.hpp"
#include "../../../dependencies/glm/gtx/quaternion.hpp"
#include "../../../dependencies/glm/gtx/dual_quaternion.hpp"
#include "../../../dependencies/glm/gtc/type_ptr.hpp"
#include "../../../dependencies/glm/gtc/matrix_transform.hpp"
#include "../assimp-3.3.1/include/assimp/Importer.hpp"
#include "../assimp-3.3.1/include/assimp/scene.h"
#include "../assimp-3.3.1/include/assimp/postprocess.h"
#include "texture_loader.hpp"
#include <SDL.h>

#define PI 3.14159265359f
#define _BUFFER_ANIMATION
typedef unsigned int uint;
bool dq_skinning = false;

static float frameTime, moveSpeed, rotateSpeed;
static uint GLWindowWidth, GLWindowHeight;
static void setGLWscreen(uint w, uint h) {
	GLWindowWidth = w; GLWindowHeight = h;
}
//Struct of a simplex that may be reconstructed as a Line, Triangle and pushed back into
//up to 4 verts(tetrahedron), recalls only the last 4 added elements

static int MAX_ITERATIONS = 32;


glm::mat4 convertMatrix4x4(const aiMatrix4x4 m)
{
	glm::mat4 Matri;

	Matri[0][0] = m.a1;
	Matri[0][1] = m.b1;
	Matri[0][2] = m.c1;
	Matri[0][3] = m.d1;
	Matri[1][0] = m.a2;
	Matri[1][1] = m.b2;
	Matri[1][2] = m.c2;
	Matri[1][3] = m.d2;
	Matri[2][0] = m.a3;
	Matri[2][1] = m.b3;
	Matri[2][2] = m.c3;
	Matri[2][3] = m.d3;
	Matri[3][0] = m.a4;
	Matri[3][1] = m.b4;
	Matri[3][2] = m.c4;
	Matri[3][3] = m.d4;

	return Matri;
}

glm::mat4 convertMatrix3x3(const aiMatrix3x3 m)
{
	glm::mat4 Matri = glm::mat4(1);

	Matri[0][0] = m.a1;
	Matri[0][1] = m.b1;
	Matri[0][2] = m.c1;
	Matri[1][0] = m.a2;
	Matri[1][1] = m.b2;
	Matri[1][2] = m.c2;
	Matri[2][0] = m.a3;
	Matri[2][1] = m.b3;
	Matri[2][2] = m.c3;

	return Matri;
}
struct CharacterAction {
	bool moving, punching;
	bool block_moves;
	bool movingForward, movingBackward, movingLeft, movingRight, movingUp, movingDown, jumping, falling;
	CharacterAction() {
		movingForward =
			movingBackward =
			movingLeft =
			movingRight =
			movingUp =
			movingDown =
			jumping =
			falling =
			punching =
			moving =
			block_moves = false;
	}
};

void printGLErr() {
	GLenum e = glGetError();
		 if (e == GL_NO_ERROR)			printf("GL_NO_ERROR\n");
	else if (e == GL_INVALID_ENUM)		printf("GL_INVALID_ENUM\n");
	else if (e == GL_INVALID_VALUE)		printf("GL_INVLAID_VALUE\n");
	else if (e == GL_INVALID_OPERATION)	printf("GL_INVALID_OPERATION\n");
	else if (e == GL_INVALID_FRAMEBUFFER_OPERATION)printf("GL_INVALID_FRAMEBUFFER_OPERATION\n");
	else if (e == GL_OUT_OF_MEMORY)printf("GL_OUT_OF_MEMORY\n");
	else if (e == GL_STACK_UNDERFLOW)printf("GL_STACK_UNDERFLOW\n");
	else if (e == GL_STACK_OVERFLOW)printf("GL_STACK_OVERFLOW\n");
}

void printMat4(glm::mat4 &m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++){
			printf("%3.2f ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");
}
void printMat2x4(glm::mat2x4 &m) {
	printf("MYDQ\n");
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%3.2f ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");
}
void printDQ(glm::dualquat dq) {
	printf("GLM::DQ\n");
	printf("%3.2f %3.2f %3.2f %3.2f \n", dq.real.w, dq.real.x, dq.real.y, dq.real.z);
	printf("%3.2f %3.2f %3.2f %3.2f \n", dq.dual.w, dq.dual.x, dq.dual.y, dq.dual.z);
}
#endif //__UBG_UTILS*/
#pragma once
struct CharacterAction {
	bool moving, punching;
	bool block_moves;
	bool jumping, falling;
	glm::vec2 movement_dir;
	CharacterAction() {
		jumping =
		falling =
		punching =
		moving =
		block_moves = false;
		movement_dir = glm::vec2(0, 0);
	}
};