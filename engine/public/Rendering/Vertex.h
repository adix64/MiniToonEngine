#pragma once
#include <GL/glew.h>
#include <cstdio>

struct VertexFormat
{
	GLfloat position_x, position_y, position_z;				//pozitia unui vertex (x,y,z)	
	GLfloat normal_x, normal_y, normal_z;					//vom invata ulterior, nu este folosit in acest lab
	GLfloat tangent_x, tangent_y, tangent_z; 
	GLfloat texcoord_x, texcoord_y;							//vom invata ulterior, nu este folosit in acest lab

	VertexFormat()
	{
		position_x = position_y = position_z = 0;
		normal_x = normal_y = normal_z = 0;
		texcoord_x = texcoord_y = 0;
	}

	VertexFormat(GLfloat px, GLfloat py, GLfloat pz)
	{
		position_x = px;
		position_y = py;
		position_z = pz;

		normal_x = normal_y = normal_z = 0;

		texcoord_x = texcoord_y = 0;
	}

	VertexFormat(GLfloat px, GLfloat py, GLfloat pz, GLfloat nx, GLfloat ny, GLfloat nz)
	{
		position_x = px;
		position_y = py;
		position_z = pz;

		normal_x = nx;
		normal_y = ny;
		normal_z = nz;

		texcoord_x = texcoord_y = 0;
	}

	VertexFormat(GLfloat px, GLfloat py, GLfloat pz, GLfloat tx, GLfloat ty)
	{
		position_x = px;		position_y = py;		position_z = pz;
		texcoord_x = tx;		texcoord_y = ty;
		normal_x = normal_y = normal_z = 0;
	}

	VertexFormat(GLfloat px, GLfloat py, GLfloat pz, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat tx, GLfloat ty)
	{
		position_x = px;
		position_y = py;
		position_z = pz;

		normal_x = nx;
		normal_y = ny;
		normal_z = nz;

		texcoord_x = tx;
		texcoord_y = ty;
	}

	VertexFormat operator=(const VertexFormat &rhs)
	{
		position_x = rhs.position_x;
		position_y = rhs.position_y;
		position_z = rhs.position_z;

		normal_x = rhs.normal_x;
		normal_y = rhs.normal_y;
		normal_z = rhs.normal_z;

		texcoord_x = rhs.texcoord_x;
		texcoord_y = rhs.texcoord_y;
		return (*this);
	}
};