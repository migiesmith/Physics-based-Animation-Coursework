/*
Grant Smith (40111906)

Loads in and manages all of the links within an ik hierarchy

*/

#pragma once


#include <jsoncpp\json\json.h>
#include <iostream>
#include <fstream>

#include "Link.h"
#include "LineCollider.h"
#include "IKModels.h"

using namespace std;

class IKHierarchy
{
private:

	bool ready = false;

public:

	// Stores all links within the hierarchy
	vector<Link*> allLinks;
	// Stores the root bone of the hierarchy
	Link* rootBone;

	// Stores the endLinks of the hierarchy
	map<string, Link*> endLinks;

	// Default constructor
	IKHierarchy(){ }
	// Load in the hierarchy from filePath
	IKHierarchy(char* filepath);

	// Parses the entire JSON file
	void parseIKTree(Json::Value& json, Link* parentLink);

	// Parses a bone json object
	Link* parseBone(Json::Value jsonBone);

	// updates the hierarchy
	void update(){ rootBone->update(); }

	// render the hierarchy
	void render(mat4& PV, effect& currentEffect){
		if (rootBone && ready)
			rootBone->render(PV, currentEffect);
	}

	// Delete the hierarchy
	~IKHierarchy();
};

