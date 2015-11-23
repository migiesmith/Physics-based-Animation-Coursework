#include "TextRenderer.h"

// constructor for the text renderer
TextRenderer::TextRenderer(string fontPath, effect* shader)
{
	loadFontTexture(("fonts\\" + fontPath + ".png").c_str());
	loadCharacterMapping(("..\\resources\\textures\\fonts\\" + fontPath + ".fnt").c_str());
	this->shader = shader;
	isReady = true;
}

// Load font texture from fontPath
void TextRenderer::loadFontTexture(const char* fontPath){
	tex = Util::loadTexture(fontPath);
}

// Load font character mapping from fontPath
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

// Renders text to the screen at x, y where x and y are coordinates in the range 0 to 1
void TextRenderer::render(const mat4& orthoMVP, const string text, float x, float y){
	if (!isReady || text.size() == 0) return;

	// Bind the shader
	renderer::bind(*shader);
	// Set the mvp matrix
	glUniformMatrix4fv(
		shader->get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data
	glUniform1i(shader->get_uniform_location("tex"), 0);

	// The x and y offset from the original rendering position x, y
	int xOffset = 0;
	int yOffset = 0;
	// Create a structure to store the characters in
	struct Vertex {
		GLfloat position[3];
		GLfloat texcoord[2];
	};
	// The number of vertex objects needed
	const int tSize = text.size();
	const int NUM_VERTS = 6 * tSize;
	// The number of vertex objects needed
	Vertex* vertexdata = new Vertex[NUM_VERTS];

	// Loop through each character and add it to the vertex data
	int vertNum = 0;
	for (char c : text){
		if (c == ' '){
			// If the current character is a space, just increment the xOffset
			xOffset += (int)((characters['!']->width + characters['!']->xAdvance) * fontSize * 2.0f);
		}else if(c == '\n'){
			yOffset -= (int)getFontHeight();
			xOffset = 0;
		}else{
			// Get the next character using the current char
			TextRendChar* character = characters[c];

			// Get the size of the quad we need to make
			float quadX = character->width * fontSize;
			float quadY = character->height * fontSize;
			float xPos = (renderer::get_screen_width() / 2.0f) - quadX - (x*renderer::get_screen_width());
			float yPos = -(renderer::get_screen_height() / 2.0f) + quadY + character->yOffset * 0.5f + (y*renderer::get_screen_height());

			// Get the tex coord values for the current character quad
			float xCoord = (float)character->xPos / (float)tex.get_width();
			float yCoord = -(float)character->yPos / (float)tex.get_height();
			float w = (float)character->width / (float)tex.get_width();
			float h = -(float)character->height / (float)tex.get_height();
			
			// Add each corner to the vertex data
			vertexdata[vertNum] = { { -quadX + xOffset - xPos, quadY + yOffset - yPos, 0.0f }, { xCoord, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset - xPos, -quadY + yOffset - yPos, 0.0f }, { xCoord + w, yCoord + h } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset - xPos, quadY + yOffset - yPos, 0.0f }, { xCoord + w, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { -quadX + xOffset - xPos, quadY + yOffset - yPos, 0.0f }, { xCoord, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { -quadX + xOffset - xPos, -quadY + yOffset - yPos, 0.0f }, { xCoord, yCoord + h } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset - xPos, -quadY + yOffset - yPos, 0.0f }, { xCoord + w, yCoord + h } };
			vertNum++;

			// Increment the xOffset to move the character along
			xOffset += (int)((characters[c]->width + characters[c]->xAdvance) * fontSize);
		}
	}


	// Create and bind a VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind a BO for vertex data
	GLuint vbuffer;
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

	// copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * sizeof(Vertex), vertexdata, GL_STATIC_DRAW);

	// Delete the vertex data
	delete[] vertexdata;

	// Get the attribute positions from the shader
	int posAttrib = glGetAttribLocation(shader->get_program(), "position");
	int texAttrib = glGetAttribLocation(shader->get_program(), "tex_coord_in");

	// set up vertex attributes
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

	
	// Bind the texture
	renderer::bind(tex, 0);
	// don't render transparency below 0.2f
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	// Remder the string
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
	glDisable(GL_ALPHA_TEST);

	// Clean up the buffers and unbind the texture
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(posAttrib);
	glDisableVertexAttribArray(texAttrib);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffersARB(1, &vbuffer);

}

// (3D) Renders text in the scene at pos, right is the camera's right so that the text faces the camera
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


	// The x and y offset from the original rendering position x, y
	int xOffset = 0;
	int yOffset = 0;
	// Create a structure to store the characters in
	struct Vertex {
		GLfloat position[3];
		GLfloat texcoord[2];
	};
	// The number of vertex objects needed
	const int tSize = text.size();
	const int NUM_VERTS = 6 * tSize;
	// The number of vertex objects needed
	Vertex* vertexdata = new Vertex[NUM_VERTS];

	int vertNum = 0;
	for (char c : text){
		if (c == ' '){
			// If the current character is a space, just increment the xOffset
			xOffset += (int)((characters['!']->width + characters['!']->xAdvance) * fontSize * 2.0f);
		}
		else if (c == '\n'){
			yOffset -= (int)getFontHeight();
			xOffset = 0;
		}
		else{
			// Get the next character using the current char
			TextRendChar* character = characters[c];

			// Get the size of the quad we need to make
			float quadX = character->width * fontSize;
			float quadY = character->height * fontSize;

			// Get the tex coord values for the current character quad
			float xCoord = (float)character->xPos / (float)tex.get_width();
			float yCoord = -(float)character->yPos / (float)tex.get_height();
			float w = (float)character->width / (float)tex.get_width();
			float h = -(float)character->height / (float)tex.get_height();

			// Add each corner to the vertex data
			vertexdata[vertNum] = { { -quadX + xOffset, quadY + yOffset, 0 }, { xCoord, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset, -quadY + yOffset, 0 }, { xCoord + w, yCoord + h } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset, quadY + yOffset, 0 }, { xCoord + w, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { -quadX + xOffset, quadY + yOffset, 0 }, { xCoord, yCoord } };
			vertNum++;

			vertexdata[vertNum] = { { -quadX + xOffset, -quadY + yOffset, 0 }, { xCoord, yCoord + h } };
			vertNum++;

			vertexdata[vertNum] = { { quadX + xOffset, -quadY + yOffset, 0 }, { xCoord + w, yCoord + h } };
			vertNum++;

			// Increment the xOffset to move the character along
			xOffset += (int)((characters[c]->width + characters[c]->xAdvance) * fontSize);
		}
	}

	// Create and bind a VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind a BO for vertex data
	GLuint vbuffer;
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

	// copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * sizeof(Vertex), vertexdata, GL_STATIC_DRAW);

	// Delete the vertex data
	delete[] vertexdata;

	// Get the attribute positions from the shader
	int posAttrib = glGetAttribLocation(shader->get_program(), "position");
	int texAttrib = glGetAttribLocation(shader->get_program(), "tex_coord_in");

	// set up vertex attributes
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));


	// Bind the texture
	renderer::bind(tex, 0);
	// don't render transparency below 0.2f
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	// Remder the string
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
	glDisable(GL_ALPHA_TEST);

	// Clean up the buffers and unbind the texture
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(posAttrib);
	glDisableVertexAttribArray(texAttrib);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffersARB(1, &vbuffer);

}

// Returns the font height
float TextRenderer::getFontHeight(){
	return (characters['T']->height + characters['T']->yOffset)* fontSize * 2.2f;
}

// Returns the string width
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