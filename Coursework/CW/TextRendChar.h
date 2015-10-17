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

	TextRendChar(int id, int xPos, int yPos, int width, int height, int xOffset, int yOffset, int xAdvance, int page, int channel){
		this->id = id;
		this->xPos = xPos;
		this->id = id;
		this->yPos = yPos;
		this->width = width;
		this->height = height;
		this->xOffset = xOffset;
		this->yOffset = yOffset;
		this->xAdvance = xAdvance;
		this->page = page;
		this->channel = channel;
	}

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

	~TextRendChar();
};

