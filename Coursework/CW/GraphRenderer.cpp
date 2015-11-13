#include "GraphRenderer.h"


GraphRenderer::GraphRenderer(TextRenderer* textRenderer, effect* shader, const float& width, const float& height)
{
	this->textRenderer = textRenderer;
	this->shader = shader;
	pushData(0.0f);
	this->width = width;
	this->height = height;
}

void GraphRenderer::pushData(const float y){
	if(y >= 0)
		data.push_back(y);
	if ((int)data.size() > limit)
		data.erase(data.begin());
}

float GraphRenderer::getMaxDataValue(){
	float max = numeric_limits<float>::min();
	for (float d : data){
		if (d > max) max = d;
	}
	return max;
}

float GraphRenderer::getAverageDataValue(){
	float sum = 0.0f;
	for (float d : data){
		sum += d;
	}
	return sum / data.size();
}

float GraphRenderer::getMinDataValue(){
	float min = numeric_limits<float>::max();
	for (float d : data){
		if (d <= min) min = d;
	}
	return min;
}

void GraphRenderer::render(const mat4 &orthoMVP, const float x, const float y){
	float xPos = -(renderer::get_screen_width() / 2.0f) + x*renderer::get_screen_width() + textRenderer->getStringWidth(xText);
	float yPos = renderer::get_screen_height() - ( (renderer::get_screen_height() / 2.0f) + (y*renderer::get_screen_height())) - height;
	renderer::bind(*shader);
	glUniformMatrix4fv(
		shader->get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data

	// Set bar colour
	glUniform4fv(shader->get_uniform_location("colour"), 1, value_ptr(barColour));
	glBegin(GL_LINES);
	glVertex3f(xPos, yPos, 0);
	glVertex3f(xPos, yPos + height, 0);
	glVertex3f(xPos, yPos, 0);
	glVertex3f(xPos + width, yPos, 0);
	glEnd();

	// Set line colour
	glUniform4fv(shader->get_uniform_location("colour"), 1, value_ptr(lineColour));
	glBegin(GL_LINE_STRIP);
	int i = 1;
	for (float d : data){
		float yPosVal = (d - getAverageDataValue())*((height*0.5f)/getMaxDataValue())+ height*0.5f ;
		glVertex3f(xPos + (width / limit) * i, yPos + (yPosVal >= 0 ? yPosVal : 0), 0);
		i++;
	}
	glEnd();

	// Render x-axis text
	mat4 xTextM = Util::translationMat4(vec3(textRenderer->getStringWidth(xText) - textRenderer->getStringWidth(xText), -height * 0.5f + textRenderer->getFontHeight()*0.5f, 0));
	textRenderer->render(orthoMVP * xTextM, xText, x, y);

	// Render y-axis text
	mat4 yTextM = Util::translationMat4(vec3(textRenderer->getStringWidth(xText) + width * 0.5f - textRenderer->getStringWidth(yText) / 2.0f, -4 - height, 0));
	textRenderer->render(orthoMVP * yTextM, yText, x, y);

	// Render max value
	mat4 maxTextM = Util::translationMat4(vec3(textRenderer->getStringWidth(xText) + width, -height * 0.5f + textRenderer->getFontHeight()*0.5f + textRenderer->getFontHeight(), 0));
	std::ostringstream maxStr;
	maxStr << std::setprecision(2) << getMaxDataValue();
	string maxValue = maxStr.str() + " max";
	textRenderer->render(orthoMVP * maxTextM, maxValue, x, y);

	// Render avg value
	mat4 avgTextM = Util::translationMat4(vec3(textRenderer->getStringWidth(xText) + width, -height * 0.5f + textRenderer->getFontHeight()*0.5f, 0));
	std::ostringstream avgStr;
	avgStr << std::setprecision(2) << getAverageDataValue();
	string averageValue = avgStr.str() + " avg";
	textRenderer->render(orthoMVP * avgTextM, averageValue, x, y);

	// Render min value
	mat4 minTextM = Util::translationMat4(vec3(textRenderer->getStringWidth(xText) + width, -height * 0.5f + textRenderer->getFontHeight()*0.5f - textRenderer->getFontHeight(), 0));
	std::ostringstream minStr;
	minStr << std::setprecision(2) << getMinDataValue();
	string minValue = minStr.str() + " min";
	textRenderer->render(orthoMVP * minTextM, minValue, x, y);
}



void GraphRenderer::setXText(const string& xText){
	this->xText = xText;
}
void GraphRenderer::setYText(const string& yText){
	this->yText = yText;
}
void GraphRenderer::setText(const string& xText, const string& yText){
	setXText(xText);
	setYText(yText);
}


GraphRenderer::~GraphRenderer()
{
}
