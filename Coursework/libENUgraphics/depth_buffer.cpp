#include "stdafx.h"

#include "depth_buffer.h"
#include "util.h"

namespace graphics_framework
{
	// Creates a depth buffer object
	depth_buffer::depth_buffer(GLuint width, GLuint height) throw (...)
		: _width(width), _height(height)
	{
		// Create depth texture
		_depth = texture(width, height);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _depth.get_id());
		// Check for error
		if (CHECK_GL_ERROR)
		{
			// Display error
			std::cerr << "ERROR - building depth buffer" << std::endl;
			std::cerr << "Could not allocate depth texture with OpenGL" << std::endl;
			// Throw exception
			throw std::runtime_error("Error creating depth texture with OpenGL");
		}

		// Create the depth image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		// Check if error
		if (CHECK_GL_ERROR)
		{
			// Display error
			std::cerr << "ERROR - building depth buffer" << std::endl;
			std::cerr << "Could not create depth image data with OpenGL" << std::endl;
			// Throw exception
			throw std::runtime_error("Error creating depth texture with OpenGL");
		}

		// Set texture properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		CHECK_GL_ERROR; // Not considered fatal here

		// Create and set up the FBO
		glGenFramebuffers(1, &_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _buffer);
		// Check for errors
		if (CHECK_GL_ERROR)
		{
			// Display error
			std::cerr << "ERROR - building depth buffer" << std::endl;
			std::cerr << "Could not allocate frame buffer with OpenGL" << std::endl;
			// Delete framebuffer
			glDeleteFramebuffers(1, &_buffer);
			// Throw exception
			throw std::runtime_error("Error creating depth buffer with OpenGL");
		}

		// Attach the frame and depth buffers
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth.get_id(), 0);
		// Check for errors
		if (CHECK_GL_ERROR)
		{
			// Display error
			std::cerr << "ERROR - building depth buffer" << std::endl;
			std::cerr << "Could not attach texture to frame buffer" << std::endl;
			// Delete frame buffer
			glDeleteFramebuffers(1, &_buffer);
			// Throw exception
			throw std::exception("Error creating depth buffer with OpenGL");
		}

		// Set draw buffer
		glDrawBuffer(GL_NONE);
		// Check for errors
		if (CHECK_GL_ERROR)
		{
			// Display error
			std::cerr << "ERROR - building depth buffer" << std::endl;
			std::cerr << "Could not set draw buffer" << std::endl;
			// Delete frame buffer
			glDeleteFramebuffers(1, &_buffer);
			// Throw exception
			throw std::exception("Error creating depth buffer with OpenGL");
		}

		// Unbind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CHECK_GL_ERROR; // Non-fatal here

		// Log
		std::clog << "LOG - depth bufer built" << std::endl;
	}
}