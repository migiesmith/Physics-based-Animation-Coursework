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

class TextRenderer
{

private:

	// Size of the font read in from the file (stored as an inverse to avoid division)
	float invInFontSize = 0.0f;
	// Size of the font relative to the read in size
	float fontSize = 1.0f;

	// Used to check if the font is ready to be used
	bool isReady = false;

public:

	// Texture for the font
	texture tex;
	// Information about each character (e.g. the dimensions on the image)
	map<char, TextRendChar*> characters;


	TextRenderer(string fontPath);

	void loadFontTexture(const char* fontPath);
	void loadCharacterMapping(const char* fontPath);
	void render(const string text, float x, float y);

	void setFontSize(const float sizeInPt);

	~TextRenderer();

};

