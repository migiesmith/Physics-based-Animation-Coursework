/*

	Grant Smith (40111906)

	Creates an frame buffer that stores both the colour and depth of anything rendered to it

*/

#include "FrameBuffer.h"

using namespace std;

namespace graphics_framework{

	FrameBuffer::FrameBuffer(const GLuint &width, const GLuint &height) : FrameBuffer(width, height, GL_RGB){
	}

	FrameBuffer::FrameBuffer(const GLuint &width, const GLuint &height, GLuint type) : _bufferWidth(width), _bufferHeight(height){
		// Generate a Frame Buffer
		glGenFramebuffers(1, &_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _buffer);

		// Creates and attatches the Depth Buffer
		createDepthBuffer(width, height);

		// Creates and attatches the Colour Buffer
		createColourBuffer(width, height, type);

		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		
		// Check if the frame buffer was created correctly
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "Failed to create Frame Buffer ( id = " << _buffer << " )" << endl;

	}

	void FrameBuffer::createDepthBuffer(const GLuint &width, const GLuint &height){
		/*
		Set up the Depth Buffer
		*/
		// Generate the Depth Buffer
		glGenTextures(1, &_depthBuffer);
		// Bind the generated texture
		glBindTexture(GL_TEXTURE_2D, _depthBuffer);
		// Send OpenGL an empty texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		// Set texture filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// Set to clamp to prevent anomolies at the edge of the depth buffer
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Set depth comparison mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		// Attach the Depth Buffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthBuffer, 0);

	}

	void FrameBuffer::createColourBuffer(const GLuint &width, const GLuint &height, GLuint type){
		/*
		Set up the Colour Buffer
		*/
		// Generate a texture for rendering to
		glGenTextures(1, &_colourBuffer);
		// Bind the generated texture
		glBindTexture(GL_TEXTURE_2D, _colourBuffer);
		// Send OpenGL an empty texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, type, GL_UNSIGNED_BYTE, 0);
		// Generate MipMaps for the texture
		glGenerateMipmap(GL_TEXTURE_2D);
		// Set texture filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);


		// Set the colour attachment
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colourBuffer, 0);
	}

	// Saves the FrameBuffer
	void FrameBuffer::save(const string &filename) const
	{
		// Save the Screen

		// Allocate memory to read image data into
		BYTE *pixels = new BYTE[4 * _bufferWidth * _bufferHeight];
		// Read the pixel data from the pixel data
		glReadPixels(0, 0, _bufferWidth, _bufferHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels);
		// Create an image from the pixels
		FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, _bufferWidth, _bufferHeight, 3 * _bufferWidth, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
		// Save the image
		FreeImage_Save(FIF_BMP, image, filename.c_str(), 0);
		// Unload the image
		FreeImage_Unload(image);
		// Deallocate the memory used to store the image data
		delete[] pixels;
	}

}