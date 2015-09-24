/*

Grant Smith (40111906)

TODO

*/

#pragma once
#include "Collider.h"
#include "CubeCollider.h"
#include "IntersectionData.h"
#include "Util.h"

using namespace Util;

class SphereCollider :
	public Collider
{
private:

public:

	float radius = 0.0f;

	SphereCollider(vec3 position, float radius) : Collider(position, ColliderTypes::SPHERE){
		this->radius = radius;
	}
	

	IntersectionData intersects(Collider* other, float velocity);
	~SphereCollider();
};

