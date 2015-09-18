#pragma once
#include "Collider.h"
#include "IntersectionData.h"

class SphereCollider :
	public Collider
{
private:
	float radius = 0.0f;

public:

	SphereCollider(vec3 position, float radius) : Collider(position, ColliderTypes::SPHERE){
		this->radius = radius;
	}

	IntersectionData intersects(Collider other);
	~SphereCollider();
};

