/*

Grant Smith (40111906)

A particle that can be emitted by a particle emitter

*/
#pragma once

#include <renderer.h>
#include "Util.h"
#include "SphereCollider.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class Particle : public vec3
{
public:

	// The collider of this particle
	SphereCollider* collider = NULL;
	// The duration of this particle
	float lifeTime;
	// flag to determine if the particle is alive or not
	bool isAlive;

	// Particle constructor
	Particle(const vec3 v, const float mass);
	Particle(const float x, const float y, const  float z, const float mass);

	// Apply a force
	void addForce(const vec3& f);
	// Awaken the particle for updating and rendering
	void awaken(const vec3& v, const vec3& force, const float lifeTime);
	void update(const float delta_time);

	// set the particles life time
	void setLifeTime(const float t);
	// set the particles position
	void setPosition(const vec3& v);
	// Set the mass of the particle
	void setMass(const float m);
	
	// Delete the particle
	~Particle(){}
};

