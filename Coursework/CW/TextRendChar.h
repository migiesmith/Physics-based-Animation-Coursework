/*

Grant Smith (40111906) - 23/11/2015

Stores the data about each character loaded for the text renderer

*/

#pragma once

using namespace std;

class TextRendChar
{
public:

	int 
		id,
		xPos,
		yPos,
		width,
		height,
		xOffset,
		yOffset,
		xAdvance,
		page,
		channel;

	// Constructor for the character
	TextRendChar(int id, int xPos, int yPos, int width, int height, int xOffset, int yOffset, int xAdvance, int page, int channel){
		this->id = id;
		this->xPos = xPos;
		this->yPos = yPos;
		this->width = width;
		this->height = height;
		this->xOffset = xOffset;
		this->yOffset = yOffset;
		this->xAdvance = xAdvance;
		this->page = page;
		this->channel = channel;
	}

	// Character information for debugging
	string toString(){
		ostringstream os;
		os << "char '" << (char)id << "': " <<
			id << ", " <<
			xPos << ", " <<
			yPos << ", " <<
			width << ", " <<
			height << ", " <<
			xOffset << ", " <<
			yOffset << ", " <<
			xAdvance << ", " <<
			page << ", " <<
			channel;
		return os.str();
	}

	// delete the character
	~TextRendChar(){}
};

