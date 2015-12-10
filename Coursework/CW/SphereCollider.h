/*

Grant Smith (40111906) - 9/11/2015

The collider that handles sphere related intersection (sphere-sphere, sphere-plane, sphere-cube, etc.)

*/

#define DBG_HALT __asm {int 3}
#if defined(DEBUG) | defined(_DEBUG)
#define DBG_ASSERT(exp) {if(!(exp)) {DBG_HALT;}}
#else
#define DBG_ASSERT(exp) {}
#endif

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
		DBG_ASSERT(radius > 0.0f);
		this->radius = radius;
	}
	
	// Checks for collision between the sphere and collider other
	void intersects(Collider& other, const vec3& velocity, IntersectionData& data) ;

	// Delete the sphere collider
	~SphereCollider();
};

