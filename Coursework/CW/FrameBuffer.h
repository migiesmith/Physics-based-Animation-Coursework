/*

	Grant Smith (40111906)

	Creates an frame buffer that stores both the colour and depth of anything rendered to it

*/

#pragma once

#include "stdafx.h"
#include "texture.h"
#include <FreeImage\FreeImage.h>

namespace graphics_framework
{
	class FrameBuffer
	{
	private:
		// The OpenGL ID of the Frame Buffer
		GLuint _buffer;
		// The OpenGL ID of the depth texture of Frame Buffer
		GLuint _depthBuffer;
		// The OpenGL ID of the texture of the Frame Buffer
		GLuint _colourBuffer;

		// The Dimensions of the Frame Buffer
		GLuint _bufferWidth;
		GLuint _bufferHeight;

	public:

		/*
			Default constructor (used for creating a global Frame Buffer variable)
			!The other constructor must be called before the Frame Buffer is used!
		*/
		FrameBuffer(){}

		// Destroys the frame buffer
		~FrameBuffer() {
		}

		// Creates a frame buffer
		FrameBuffer(const GLuint &width, const GLuint &height);
		FrameBuffer(const GLuint &width, const GLuint &height, GLuint type) throw (...);

		// Creates the Depth Buffer for the Frame Buffer
		void createDepthBuffer(const GLuint &width, const GLuint &height);
		// Creates the Colour Buffer for the Frame Buffer
		void createColourBuffer(const GLuint &width, const GLuint &height, GLuint type);


		GLuint get_buffer(){ return _buffer; } // Returns the Frame Buffer
		GLuint get_depth(){ return _depthBuffer; } // Returns the id of the Depth texture
		GLuint get_texture(){ return _colourBuffer; } // Returns the id of the Colour Texture
		
		
		// Saves the screen to a file
		void save(const std::string &filename) const;

	};
}