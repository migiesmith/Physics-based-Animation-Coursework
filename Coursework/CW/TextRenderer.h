/*

Grant Smith (40111906) - 23/11/2015

The renderer used to render text on the screen (2D + 3D)

*/

#pragma once

#include <texture.h>
#include <mesh.h>
#include <renderer.h>
#include <iostream>

#include <tinyxml\tinyxml.h>

#include "TextRendChar.h"
#include "Util.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;
using namespace Util;

class TextRenderer
{

private:

	// Size of the font read in from the file (stored as an inverse to avoid division)
	float invInFontSize = 0.0f;
	// Size of the font relative to the read in size
	float fontSize = 1.0f;

	// Used to check if the font is ready to be used
	bool isReady = false;

	// a pointer to the shader used by the text renderer
	effect* shader;

public:

	// Texture for the font
	texture tex;
	// Information about each character (e.g. the dimensions on the image)
	map<char, TextRendChar*> characters;

	// constructor for the text renderer
	TextRenderer(string fontPath, effect* shader);

	// Load font texture from fontPath
	void loadFontTexture(const char* fontPath);
	// Load font character mapping from fontPath
	void loadCharacterMapping(const char* fontPath);
	// (2D) Renders text to the screen at x, y where x and y are coordinates in the range 0 to 1
	void render(const mat4& orthoMVP, const string text, float x, float y);
	// (3D) Renders text in the scene at pos, right is the camera's right so that the text faces the camera
	void render3D(const mat4& MVP, const vec3& right, const string text, vec3& pos);
	
	// Returns the font height
	float getFontHeight();
	// Returns the string width
	float getStringWidth(const string& text);
	// Takes in a pt size for the font and sets the fontSize value determined by the screen size and the size of the font from the file
	void setFontSize(const float sizeInPt);

	// Delete the text renderer
	~TextRenderer();

};

