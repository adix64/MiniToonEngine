#pragma once
#include <System/System.h>
#include <SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>


class Texture
{
private:
	unsigned tex;
public:
	static GLuint png_texture_load(const char * file_name, int * width, int * height, GLenum internalFormat = GL_COMPRESSED_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

	static GLuint LUTtextureLoad(const char * file_name, int * width, int * height);
	
};


