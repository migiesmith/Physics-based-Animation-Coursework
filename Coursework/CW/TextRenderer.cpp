#include "TextRenderer.h"


TextRenderer::TextRenderer(string fontPath)
{
	loadFontTexture(("fonts\\" + fontPath + ".png").c_str());
	fontPath = "..\\resources\\textures\\fonts\\" + fontPath;
	loadCharacterMapping((fontPath + ".fnt").c_str());
}

void TextRenderer::loadFontTexture(const char* fontPath){
	tex = &Util::loadTexture(fontPath);
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

TextRenderer::~TextRenderer()
{
}
