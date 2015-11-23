/*

Grant Smith (40111906)

Handles line intersection within the scene

*/

#pragma once

#include "Collider.h"
#include "IntersectionData.h"

using namespace Util;

class LineCollider : public Collider
{
public:

	float radius; // radius of the collider (so it acts like a capsule)
	vec3 endPosition; // the end point of the collider

	// Constructor for the line collider
	LineCollider(vec3 position, vec3 endPosition) : Collider(position, ColliderTypes::LINE){
		this->endPosition = endPosition;
		this->radius = 0.0f;
	}

	// Constructor for the line collider
	LineCollider(vec3 position, vec3 endPosition, const float radius) : Collider(position, ColliderTypes::LINE){
		this->endPosition = endPosition;
		this->radius = radius;
	}

	// check for an intersection between this line and another collider
	void intersects(Collider& other, const vec3& velocity, IntersectionData& data);

	// Delete the line collider
	~LineCollider(){}

};

