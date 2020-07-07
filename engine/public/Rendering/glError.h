#pragma once
#include <iostream>

namespace OpenGL
{
	// -------------------------------------------------------------------------
	// Check for OpenGL Errors
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	int CheckError(const char *file, int line, bool log = true);
};

#define _DEBUG
#ifdef _DEBUG
#define CheckOpenGLError() OpenGL::CheckError(__FILE__, __LINE__)
#define SilentCheckOpenGLError() OpenGL::CheckError(__FILE__, __LINE__, false)
#else
#define CheckOpenGLError()
#define SilentCheckOpenGLError()
#endif