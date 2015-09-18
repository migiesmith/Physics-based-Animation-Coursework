#pragma once

#include "stdafx.h"

namespace graphics_framework
{
#if defined(DEBUG) | defined(_DEBUG)
	// Enables memory leak checking
	inline void set_debug()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	// Checks for any OpenGL errors
	inline bool get_GL_error(int line, const std::string& file)
	{
		// Get the current error
		GLenum error = glGetError();
		// If there is an error display message
		if (error)
		{
			// Display error
			std::cerr << "OpenGL Error: " << gluErrorString(error) << std::endl;
			std::cerr << "At line " << line << " in file " << file << std::endl;
			return true;
		}
		return false;
	}
#define CHECK_GL_ERROR get_GL_error(__LINE__, __FILE__)
#define SET_DEBUG set_debug()
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#else
#define CHECK_GL_ERROR false
#define SET_DEBUG
#endif
	// Utility function to check if a file exists
	inline bool check_file_exists(const std::string& filename)
	{
		std::ifstream file(filename);
		return file.good();
	}
}