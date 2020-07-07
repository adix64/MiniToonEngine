#pragma once
#include <string>
#include <vector>
#include <list>
#include <functional>
#include <unordered_map>
#include <GL/glew.h>

#define MAX_2D_TEXTURES		16
#define INVALID_LOC			-1

class Shader
{
public:
	Shader(const char *name);
	~Shader();

	const char *GetName() const;
	GLuint GetProgramID() const;

	void Use() const;
	unsigned int Reload();

	void AddShader(const std::string &shaderFile, GLenum shaderType);
	unsigned int CreateTransformFeedbackShader(GLint varyingsSize, const GLchar* feedbackVaryings[]);
	void ClearShaders();
	unsigned int CreateAndLink();

	void BindTexturesUnits();
	GLint GetAttributeLocation(const char *attributeName);
	GLint GetUniformLocation(const char *uniformName);

	void OnLoad(std::function<void()> onLoad);

private:
	void GetUniforms();
	static unsigned int CreateShader(const std::string &shaderFile, GLenum shaderType);
	static unsigned int CreateProgram(const std::vector<unsigned int> &shaderObjects);

public:
	GLuint program;
	std::unordered_map<std::string, GLint> uniformLocations;
	std::unordered_map<std::string, GLint> attributeLocations;
private:

	bool compileErrors;

	struct ShaderFile
	{
		std::string file;
		GLenum type;
	};

	std::string shaderName;
	std::vector<ShaderFile> shaderFiles;
	std::list<std::function<void()>> loadObservers;
};
