#include "IKHierarchy.h"


IKHierarchy::IKHierarchy(char* filepath)
{
	std::string line;
	std::ifstream myFile;
	std::ifstream config_doc(filepath, std::ifstream::binary);
	Json::Value root;
	config_doc >> root;

	parseIKTree(root, NULL);
	
	rootBone->update();

	rootBone->ignoreCollision = true;

	ready = true;
}

void IKHierarchy::parseIKTree(Json::Value& json, Link* parentLink){

	Link* parsedBone;

	if (!parentLink){
		parsedBone = rootBone = parseBone(json);
	}
	else{
		parsedBone = parseBone(json);
		parentLink->addChild(json["name"].asString(), parsedBone);
	}

	allLinks.push_back(parsedBone);

	Json::Value children = json["children"];
	if (!children.isNull()){

		if (!json["isEndLink"].isNull())
			if (json["isEndLink"].asBool()){
				endLinks[json["name"].asString()] = parsedBone;
			}
		
		for (Json::Value child : children){
			parseIKTree(child, parsedBone);
		}

	}
	else{
		endLinks[json["name"].asString()] =  parsedBone;
	}
}

Link* IKHierarchy::parseBone(Json::Value jsonBone){
	Link* link;

	Json::Value jsonAngleLimits = jsonBone["angleLimits"];
	vec3 minAngLimits = vec3(jsonAngleLimits["minX"].asFloat(), jsonAngleLimits["minY"].asFloat(), jsonAngleLimits["minZ"].asFloat());
	vec3 maxAngLimits = vec3(jsonAngleLimits["maxX"].asFloat(), jsonAngleLimits["maxY"].asFloat(), jsonAngleLimits["maxZ"].asFloat());
	
	vector<vec3>* angleLimits = new vector<vec3>{ minAngLimits, maxAngLimits };

	Json::Value rotation = jsonBone["rotation"];
	vec3 axis = vec3(rotation["axis"][0].asFloat(), rotation["axis"][1].asFloat(), rotation["axis"][2].asFloat());
	float angle = rotation["angle"].asFloat();

	float size = 1.0f;
	if (!jsonBone["size"].isNull())
		size = jsonBone["size"].asFloat();

	link = new Link(axis, angle, angleLimits, size);

	if (!jsonBone["reach"].isNull())
		link->linkReach = jsonBone["reach"].asInt();
	
	return link;
}

IKHierarchy::~IKHierarchy()
{
}
