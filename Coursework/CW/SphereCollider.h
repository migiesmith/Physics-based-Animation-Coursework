/*

Grant Smith (40111906) - 9/11/2015

The collider that handles sphere related intersection (sphere-sphere, sphere-plane, sphere-cube, etc.)

*/

#pragma once
#include "Collider.h"
#include "CubeCollider.h"
#include "PlaneCollider.h"
#include "IntersectionData.h"

using namespace Util;

class SphereCollider :
	public Collider
{
private:

public:

	float radius = 0.0f; // radius of the sphere

	// Constructor for the sphere
	SphereCollider(vec3 position, float radius) : Collider(position, ColliderTypes::SPHERE){
		this->radius = radius;
	}
	
	// Checks for collision between the sphere and collider other
	void intersects(Collider& other, const vec3& velocity, IntersectionData& data) ;

	// Delete the sphere collider
	~SphereCollider();
};

