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
	vector<Link*> allLinks;
	void resolveCollisions();

public:

	Link* rootBone;

	map<string, Link*> endLinks;

	IKHierarchy(){ }
	IKHierarchy(char* filepath);

	void parseIKTree(Json::Value& json, Link* parentLink);

	Link* parseBone(Json::Value jsonBone);

	void update(){ rootBone->update(); resolveCollisions(); }

	void render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target){
		if (rootBone &&  ready)
			rootBone->render(PV, currentEffect, endLink, target);
	}

	~IKHierarchy();
};

