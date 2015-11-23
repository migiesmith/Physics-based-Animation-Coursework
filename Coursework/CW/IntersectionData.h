/*

Grant Smith (40111906)

Stores the data about an intersection between two colliders

*/

#pragma once

#include <renderer.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

class IntersectionData
{
public:

	bool doesIntersect = false; // if there was an intersection
	vec3 direction = vec3(0.0f, 0.0f, 0.0f); // what direction a correction force should be applied in
	float amount = 0.0f; // the amount or intersection
	vec3 intersection = vec3(0.0f,0.0f,0.0f); // the intersection point

	// Default constructor
	IntersectionData(){}

	// Reset the intersection data
	void reset(){
		doesIntersect = false;
		direction = vec3(0.0f, 0.0f, 0.0f);
		amount = 0.0f;
		intersection = vec3(0.0f, 0.0f, 0.0f);
	}

	// Delete the intersection data
	~IntersectionData(){}
};

