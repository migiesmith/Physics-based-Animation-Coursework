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
				cout << json["name"].asString() << endl;
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

	cout << "min limits" << vec3ToString(minAngLimits) << endl;
	cout << "max limits" << vec3ToString(maxAngLimits) << endl;

	vector<vec3>* angleLimits = new vector<vec3>{ minAngLimits, maxAngLimits };

	//cout << jsonBone << endl;
	Json::Value rotation = jsonBone["rotation"];
	vec3 axis = vec3(rotation["axis"][0].asFloat(), rotation["axis"][1].asFloat(), rotation["axis"][2].asFloat());
	float angle = rotation["angle"].asFloat();

	float size = 1.0f;
	if (!jsonBone["size"].isNull())
		size = jsonBone["size"].asFloat();

	if (!jsonBone["size"].isNull())
		size = jsonBone["size"].asFloat();

	link = new Link(axis, angle, angleLimits, size);

	if (!jsonBone["reach"].isNull())
		link->linkReach = jsonBone["reach"].asInt();

	//link->origin = vec3(jsonBone["translation"][0].asFloat(), jsonBone["translation"][1].asFloat(), jsonBone["translation"][2].asFloat());

	return link;
}

void IKHierarchy::resolveCollisions(){
	/*
	LineCollider lCol1 = LineCollider(vec3(0, 0, 0), vec3(0, 0, 1));
	LineCollider lCol2 = LineCollider(vec3(0, 0, 0), vec3(0, 0, 1));

	IntersectionData data = IntersectionData();

	for (int i = 0; i < (int) allLinks.size() - 1; i++){
		for (int j = i + 1; j < (int) allLinks.size(); j++){

			Link* l1 = allLinks.at(i);
			Link* l2 = allLinks.at(j);
			
			if (l1->ignoreCollision || l2->ignoreCollision || l1 == l2->parent || l2 == l1->parent){
				continue;
			}

			lCol1.position = translationFromMat4(l1->m_base);
			lCol2.position = translationFromMat4(l2->m_base);

			if (magnitude(lCol1.position - lCol2.position) < 1.0f)
				continue;

			lCol1.endPosition = vec4ToVec3(l1->m_base * vec4(l1->m_length, 0, 0, 1));
			lCol2.endPosition = vec4ToVec3(l2->m_base * vec4(l2->m_length, 0, 0, 1));

			lCol1.intersects(lCol2, vec3(0,0,0), data);
			
			//TODO fix line collider

			if (data.doesIntersect){
				
			}
			
		}
	}
	*/
}

IKHierarchy::~IKHierarchy()
{
}
