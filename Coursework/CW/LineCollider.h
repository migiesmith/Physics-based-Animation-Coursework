#pragma once

#include "Collider.h"
#include "IntersectionData.h"

using namespace Util;

class LineCollider : public Collider
{
public:

	float radius;
	vec3 endPosition;

	LineCollider(vec3 position, vec3 endPosition) : Collider(position, ColliderTypes::LINE){
		this->endPosition = endPosition;
		this->radius = 0.0f;
	}

	LineCollider(vec3 position, vec3 endPosition, const float radius) : Collider(position, ColliderTypes::LINE){
		this->endPosition = endPosition;
		this->radius = radius;
	}

	void intersects(Collider& other, const vec3& velocity, IntersectionData& data);

	~LineCollider();

};

