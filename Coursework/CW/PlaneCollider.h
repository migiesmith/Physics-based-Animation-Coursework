/*

Grant Smith (40111906) - 9/11/2015

The collider that handles plane related intersection (plane-plane, plane-sphere, plane-cube, etc.)

*/

#pragma once
#include "Collider.h"
#include "CubeCollider.h"
#include "SphereCollider.h"
#include "IntersectionData.h"

using namespace Util;

class PlaneCollider : public Collider
{
public:
	
	vec3 normal;

	PlaneCollider();

	PlaneCollider(vec3 position, vec3 normal) : Collider(position, ColliderTypes::PLANE){
		this->normal = normal;
	}


	IntersectionData intersects(Collider* other, float velocity);

	~PlaneCollider();
};

