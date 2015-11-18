/*

Grant Smith (40111906) - 9/11/2015

The base collider class that all of the colliders inherit from.

*/

#pragma once

#include <renderer.h>
#include "IntersectionData.h"
#include <math.h>
#include "Util.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;


enum class ColliderTypes :short{
	CUBE,
	SPHERE,
	OBBCUBE,
	LINE,
	PLANE
};

class Collider
{
private:

	bool _solid = true;


public:
	bool staticPos = false;
	float invMass;

	vec3 velocity, force; // Velocity and Force

	vec3 position;
	ColliderTypes colliderType;

	virtual bool Collider::isSolid();
	Collider(vec3 position, ColliderTypes colliderType);
	void setPosition(vec3 position);
	void translate(vec3 translation);
	virtual void intersects(Collider& other, const vec3& velocity, IntersectionData& data){ cout << "ERROR: Collider intersect miscalled" << endl; }

	void addForce(const vec3& f);
	void update(const float delta_time);
	void setMass(const float m);

	ColliderTypes getType(){ return colliderType; }
	~Collider();
};
