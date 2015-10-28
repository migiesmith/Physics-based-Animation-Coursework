/*

Grant Smith (40111906)

// TODO 

*/
#pragma once

#include <renderer.h>
#include "Util.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class Particle : public vec3
{
private:
	float invMass;

public:

	float lifeTime;
	bool isAlive;
	vec3 velocity, force; // Velocity and Force

	Particle(const vec3 v, const float mass);
	Particle(const float x, const float y, const  float z, const float mass);

	void addForce(const vec3& f);
	void awaken(const vec3& v, const vec3& force, const float lifeTime);
	void update(const float delta_time);
	void render();

	void setLifeTime(const float t);
	void setPosition(const vec3& v);
	void setMass(const float m);
	
	~Particle();
};

