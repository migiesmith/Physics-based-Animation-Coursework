#include "TextRenderer.h"

TextRenderer::TextRenderer(){}

TextRenderer::TextRenderer(string fontPath)
{
	loadFontTexture(("fonts\\" + fontPath + ".png").c_str());
	loadCharacterMapping(("..\\resources\\textures\\fonts\\" + fontPath + ".fnt").c_str());
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

void TextRenderer::render(const string text){
	if (!isReady) return;

	int xOffset = 0;
	geometry screenQuadGeom = geometry();
	vector<vec3> positions;
	vector<vec3> texCoords;

	for (char c : text){
		if (c == ' '){
			// If the current character is a space, just increment the xOffset
			xOffset += (characters['!']->width + characters['!']->xAdvance) * fontScale * 2.0f;
		}else{
			// Get the next character using the current char
			TextRendChar* character = characters[c];

			// Get the size of the quad we need to make
			float quadX = character->width * fontScale;
			float quadY = character->height * fontScale;

			// Push the current character's quad onto the positions vector
			positions.push_back(vec3(-quadX + xOffset, quadY, 0.0f));
			positions.push_back(vec3(quadX + xOffset, -quadY, 0.0f));
			positions.push_back(vec3(quadX + xOffset, quadY, 0.0f));

			positions.push_back(vec3(-quadX + xOffset, quadY, 0.0f));
			positions.push_back(vec3(-quadX + xOffset, -quadY, 0.0f));
			positions.push_back(vec3(quadX + xOffset, -quadY, 0.0f));

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
			xOffset += (characters[c]->width + characters[c]->xAdvance) * fontScale;
		}
	}

	// Create the mesh for rendering the screen to
	screenQuadGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screenQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// Remder the string
	renderer::bind(tex, 0);
	renderer::render(screenQuadGeom);
	
}

TextRenderer::~TextRenderer()
{
}
