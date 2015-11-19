#pragma once


#include <jsoncpp\json\json.h>
#include <iostream>
#include <fstream>

#include "Link.h"
#include "LineCollider.h"

using namespace std;

class IKHierarchy
{
private:

	bool ready = false;

public:

	vector<Link*> allLinks;
	Link* rootBone;

	map<string, Link*> endLinks;

	IKHierarchy(){ }
	IKHierarchy(char* filepath);

	void parseIKTree(Json::Value& json, Link* parentLink);

	Link* parseBone(Json::Value jsonBone);

	void update(){ rootBone->update(); }

	void render(mat4& PV, effect& currentEffect){
		if (rootBone && ready)
			rootBone->render(PV, currentEffect);
	}

	~IKHierarchy();
};

