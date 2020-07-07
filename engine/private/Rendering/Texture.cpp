#include <climits>
#include <Rendering/Texture.h>
#define STB_IMAGE_IMPLEMENTATION  
#include "../../dependencies/stb/stb_image.h"
#include "../../dependencies/stb/stb_image_write.h"

GLuint Texture::png_texture_load(const char * file_name, int * width, int * height, GLenum internalFormat, GLenum format, GLenum type)
{
	printf("[TEXTURE LOADER] Loading '%s'...\n", file_name);
	int temp_width, temp_height, temp_ch;
	unsigned char * image_data = stbi_load(file_name, &temp_width, &temp_height, &temp_ch, 0);
	unsigned char tmp;
	for (int i = 0; i < temp_height / 2; i++)
	{
		for (int j = 0; j < temp_width; j++)
		{
			for (int c = 0; c < temp_ch; c++)
			{
				tmp = image_data[temp_ch * ((temp_width * i) + j) + c];
				image_data[temp_ch * ((temp_width * i) + j) + c] = 
					image_data[temp_ch * ((temp_width * (temp_height - 1 - i)) + j) + c];
				image_data[temp_ch * ((temp_width * (temp_height - 1 - i)) + j) + c] = tmp; 
			}
		}
	}
	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//compressed
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, temp_width, temp_height, 0, format, type, image_data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp_width, temp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	// clean up
	stbi_image_free(image_data);
	return texture;
}

GLuint Texture::LUTtextureLoad(const char * file_name, int * width, int * height)
{
	int temp_width, temp_height, temp_ch;
	unsigned char * image_data = stbi_load(file_name, &temp_width, &temp_height, &temp_ch, 0);
	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);
	//compressed
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, temp_width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp_width, temp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	// clean up
	stbi_image_free(image_data);
	return texture;
}