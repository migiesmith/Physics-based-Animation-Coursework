#pragma once

#include "TextRenderer.h"
#include <iomanip>

class GraphRenderer
{
private:
	TextRenderer* textRenderer;
	effect* shader;
	vector<float> data;
	int limit = 80;
	string xText = "";
	string yText = "";

	vec4 barColour = vec4(1,1,1,1);
	vec4 lineColour = vec4(1, 0, 0,1);

	float getMaxDataValue();
	float getAverageDataValue();
	float getMinDataValue();

public:

	float width, height;

	GraphRenderer(TextRenderer* textRenderer, effect* shader, const float& width, const float& height);

	void render(const mat4 &orthoMVP, const float x, const float y);

	void pushData(const float y);
	void setLimit(const int& limit){ this->limit = limit; }
	void setXText(const string& xText);
	void setYText(const string& yText);
	void setText(const string& xText, const string& yText);
	void setBarColour(const vec4& colour){ this->barColour = colour; }
	void setLineColour(const vec4& colour){ this->lineColour = colour; }

	~GraphRenderer();
};

