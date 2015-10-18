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
public:

	texture tex;
	map<char, TextRendChar*> characters;

	float fontScale = 1.0f;

	bool isReady = false;

	TextRenderer();
	TextRenderer(string fontPath);

	void loadFontTexture(const char* fontPath);
	void loadCharacterMapping(const char* fontPath);
	void render(const string text);

	~TextRenderer();

};

