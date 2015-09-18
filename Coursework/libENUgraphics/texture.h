#pragma once

#include "stdafx.h"

namespace graphics_framework
{
	/*
	Stores the image information for texturing objects
	*/
	class texture
	{
	private:
		// The OpenGL ID of the texture data
		GLuint _id;
		// The width of the texture
		GLuint _width;
		// The height of the texture
		GLuint _height;
		// The type of the texture
		GLenum _type;
	public:
		// Default constructor
		texture() : _id(0) { }
		// Creates an empty texture of the given width and height
		texture(GLuint width, GLuint height) throw (...);
		// Loads a texture from the given filename
		texture(const std::string &filename) throw (...);
		/*
		Loads a texture from the given filename with mipmaps and anisotropic 
		filtering determined by the user.
		*/
		texture(const std::string &filename, bool mipmaps, bool anisotropic) throw (...);
		// Creates a texture from the colour data provided
		texture(const std::vector<glm::vec4> &data, GLuint width, GLuint height) throw (...);
		/*
		Creates a texture from the colour data provided and with user defined
		mipmaps and anisotropic filtering
		*/
		texture(const std::vector<glm::vec4> &data, GLuint width, GLuint height, bool mipmaps, bool anisotropic) throw (...);
		// Default copy constructor and assignment operator
		texture(const texture &other) = default;
		texture& operator=(const texture &rhs) = default;
		// Destroys the texture
		~texture() { }
		// Gets the OpenGL ID of the texture
		GLuint get_id() const { return _id; }
		// Gets the width of the texture
		GLuint get_width() const { return _width; }
		// Gets the height of the texture
		GLuint get_height() const { return _height; }
		// Gets the type of the texture
		GLuint get_type() const { return _type; }
	};
}