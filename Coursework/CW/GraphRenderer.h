/*

Grant Smith (40111906) - 23/11/2015

The renderer used to render the fps graph

*/

#pragma once

#include "TextRenderer.h"
#include <iomanip>

class GraphRenderer
{
private:
	TextRenderer* textRenderer; // a pointer to the text renderer
	effect* shader; // a pointer to the shader used
	vector<float> data; // a store for the fps data
	int limit = 80; // the limit of data
	string xText = ""; // x axis text
	string yText = ""; // y axis text

	vec4 barColour = vec4(1,1,1,1); // colour of the bars
	vec4 lineColour = vec4(1, 0, 0,1); // colour of the data line

	float getMaxDataValue(); // Returns the max highest value
	float getAverageDataValue(); // returns the average data value
	float getMinDataValue(); // returns the lowest data value

public:

	// the width and height of the graph
	float width, height;

	// Constructor for the graph
	GraphRenderer(TextRenderer* textRenderer, effect* shader, const float& width, const float& height);

	// Render the graph
	void render(const mat4 &orthoMVP, const float x, const float y);

	// Push data onto the graph
	void pushData(const float y);
	// set the data limit
	void setLimit(const int& limit){ this->limit = limit; }
	// set the x axis text
	void setXText(const string& xText);
	// set the y axis text
	void setYText(const string& yText);
	// set the x and y axis text
	void setText(const string& xText, const string& yText);
	// set the bar colour
	void setBarColour(const vec4& colour){ this->barColour = colour; }
	// set the data line colour
	void setLineColour(const vec4& colour){ this->lineColour = colour; }

	// Delte the graph renderer
	~GraphRenderer(){}
};

