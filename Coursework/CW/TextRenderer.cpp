#include "TextRenderer.h"

TextRenderer::TextRenderer(string fontPath, effect* shader)
{
	loadFontTexture(("fonts\\" + fontPath + ".png").c_str());
	loadCharacterMapping(("..\\resources\\textures\\fonts\\" + fontPath + ".fnt").c_str());
	this->shader = shader;
	isReady = true;
}

void TextRenderer::loadFontTexture(const char* fontPath){
	tex = Util::loadTexture(fontPath);
}

void TextRenderer::loadCharacterMapping(const char* fontPath){
	TiXmlDocument doc(fontPath);
	if (doc.LoadFile()){
		TiXmlHandle hDoc(&doc);
		TiXmlElement *pRoot, *pParm;

		pRoot = doc.FirstChildElement("font");
		if (pRoot){
			invInFontSize = 1.0f / stof(pRoot->FirstChildElement("info")->Attribute("size"));

			pParm = pRoot->FirstChildElement("chars");
			cout << "LOG - " << pParm->Attribute("count") << " characters loaded from " << fontPath << endl;
			pParm = pParm->FirstChildElement("char");
			while (pParm)
			{
				int id = atoi(pParm->Attribute("id"));
				int x = atoi(pParm->Attribute("x"));
				int y = atoi(pParm->Attribute("y"));
				int width = atoi(pParm->Attribute("width"));
				int height = atoi(pParm->Attribute("height"));
				int xOffset = atoi(pParm->Attribute("xoffset"));
				int	yOffset = atoi(pParm->Attribute("yoffset"));
				int	xAdvance = atoi(pParm->Attribute("xadvance"));
				int	page = atoi(pParm->Attribute("page"));
				int	channel = atoi(pParm->Attribute("chnl"));

				characters[(char)id] = new TextRendChar(id, x, y, width, height, xOffset, yOffset, xAdvance, page, channel);
				pParm = pParm->NextSiblingElement("char");
				//cout << characters[(char)id]->toString() << endl;
			}
		}
		
	}
}

// Renders text to the screen at x, y where x and y are coordinates in the ran 0 to 1
void TextRenderer::render(const mat4& orthoMVP, const string text, float x, float y){
	if (!isReady || text.size() == 0) return;

	renderer::bind(*shader);
	glUniformMatrix4fv(
		shader->get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data
	glUniform1i(shader->get_uniform_location("tex"), 0);

	int xOffset = 0;
	geometry screenQuadGeom = geometry();
	vector<vec3> positions;
	vector<vec3> texCoords;

	for (char c : text){
		if (c == ' '){
			// If the current character is a space, just increment the xOffset
			xOffset += (int)((characters['!']->width + characters['!']->xAdvance) * fontSize * 2.0f);
		}else{
			// Get the next character using the current char
			TextRendChar* character = characters[c];

			// Get the size of the quad we need to make
			float quadX = character->width * fontSize;
			float quadY = character->height * fontSize;
			float xPos = (renderer::get_screen_width() / 2.0f) - quadX - (x*renderer::get_screen_width());
			float yPos = -(renderer::get_screen_height() / 2.0f) + quadY + character->yOffset * 0.5f + (y*renderer::get_screen_height());

			// Push the current character's quad onto the positions vector
			positions.push_back(vec3(-quadX + xOffset - xPos, quadY - yPos, 0.0f));
			positions.push_back(vec3(quadX + xOffset - xPos, -quadY - yPos, 0.0f));
			positions.push_back(vec3(quadX + xOffset - xPos, quadY - yPos, 0.0f));

			positions.push_back(vec3(-quadX + xOffset - xPos, quadY - yPos, 0.0f));
			positions.push_back(vec3(-quadX + xOffset - xPos, -quadY - yPos, 0.0f));
			positions.push_back(vec3(quadX + xOffset - xPos, -quadY - yPos, 0.0f));

			// Get the tex coord values for the current character quad
			float xCoord = (float)character->xPos / (float)tex.get_width();
			float yCoord = -(float)character->yPos / (float)tex.get_height();
			float w = (float)character->width / (float)tex.get_width();
			float h = -(float)character->height / (float)tex.get_height();

			// Push the current character's texcoords onto the positions vector
			texCoords.push_back(vec3(xCoord, yCoord, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord + h, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord, 0.0f));

			texCoords.push_back(vec3(xCoord, yCoord, 0.0f));
			texCoords.push_back(vec3(xCoord, yCoord + h, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord + h, 0.0f));

			// Increment the xOffset to move the character along
			xOffset += (int)((characters[c]->width + characters[c]->xAdvance) * fontSize);
		}
	}

	// Create the mesh for rendering the screen to
	screenQuadGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screenQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// Remder the string
	renderer::bind(tex, 0);
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	renderer::render(screenQuadGeom);
	glDisable(GL_ALPHA_TEST);
	
}

void TextRenderer::render3D(const mat4& MVP, const vec3& right, const string text, vec3& pos){
	if (!isReady || text.size() == 0) return;

	renderer::bind(*shader);
	graphics_framework::transform t;
	t.translate(pos);
	if (abs(dot(right, vec3(1, 0, 0))) != 1.0f){
		t.rotate(normalize(angleAxis(-acos(fmin(fmax(dot(right, vec3(1, 0, 0)), -1.0f), 1.0f)), normalize(cross(right, vec3(1, 0, 0))))));
	}
	t.scale = vec3(0.01f, 0.01f, 0.01f);
	
	glUniformMatrix4fv(
		shader->get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(MVP * t.get_transform_matrix())); // Pointer to matrix data
	glUniform1i(shader->get_uniform_location("tex"), 0);


	int xOffset = 0;
	geometry screenQuadGeom = geometry();
	vector<vec3> positions;
	vector<vec3> texCoords;

	for (char c : text){
		if (c == ' '){
			// If the current character is a space, just increment the xOffset
			xOffset += (int)((characters['!']->width + characters['!']->xAdvance) * fontSize * 2.0f);
		}
		else{
			// Get the next character using the current char
			TextRendChar* character = characters[c];

			// Get the size of the quad we need to make
			float quadX = character->width * fontSize;
			float quadY = character->height * fontSize;

			// Push the current character's quad onto the positions vector
			positions.push_back(vec3(-quadX + xOffset, quadY , 0));
			positions.push_back(vec3(quadX + xOffset , -quadY , 0));
			positions.push_back(vec3(quadX + xOffset , quadY , 0));

			positions.push_back(vec3(-quadX + xOffset , quadY , 0));
			positions.push_back(vec3(-quadX + xOffset , -quadY, 0));
			positions.push_back(vec3(quadX + xOffset , -quadY , 0));

			// Get the tex coord values for the current character quad
			float xCoord = (float)character->xPos / (float)tex.get_width();
			float yCoord = -(float)character->yPos / (float)tex.get_height();
			float w = (float)character->width / (float)tex.get_width();
			float h = -(float)character->height / (float)tex.get_height();

			// Push the current character's texcoords onto the positions vector
			texCoords.push_back(vec3(xCoord, yCoord, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord + h, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord, 0.0f));

			texCoords.push_back(vec3(xCoord, yCoord, 0.0f));
			texCoords.push_back(vec3(xCoord, yCoord + h, 0.0f));
			texCoords.push_back(vec3(xCoord + w, yCoord + h, 0.0f));

			// Increment the xOffset to move the character along
			xOffset += (int)((characters[c]->width + characters[c]->xAdvance) * fontSize);
		}
	}

	// Create the mesh for rendering the screen to
	screenQuadGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screenQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// Remder the string
	renderer::bind(tex, 0);
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	renderer::render(screenQuadGeom);
	glDisable(GL_ALPHA_TEST);

}

float TextRenderer::getFontHeight(){
	return (characters['T']->height + characters['T']->yOffset)* fontSize * 2.0f;
}

float TextRenderer::getStringWidth(const string& text){
	float sum = 0.0f;
	for (char c : text){
		sum += ((characters[c]->width + characters[c]->xAdvance) * fontSize);
	}
	return sum;
}

// Takes in a pt size for the font and sets the fontSize value determined by the screen size and the size of the font from the file
void TextRenderer::setFontSize(const float sizeInPt){
	fontSize = (float)renderer::get_screen_width() / (float)renderer::get_screen_height();
	fontSize *= sizeInPt * invInFontSize;
}

TextRenderer::~TextRenderer()
{
}
