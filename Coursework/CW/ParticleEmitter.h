/*
Grant Smith (40111906)

Manages the emitting of particles

*/

#pragma once

#include "Particle.h"
#include "SceneObject.h"
#include "SphereCollider.h"
#include "SPGrid.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class ParticleEmitter : public vec3
{
protected:
	vector<Particle> particles; // List of particles
	float lifeTime = 5.0f; // life time of the particles
	float emitTimer = 0.5f; // timer for emitting
	int columns, rows; // the x,y count of the texture (e.g. whether its 2 by 2 or 3 by 3, etc)
	vec4 colour = vec4(1, 1, 1, 1); // colour of the particles

public:

	effect* particleShader; // A pointer to the shader used by the emitter
	texture* tex; // A pointer to the texture used by the emitter
	vec3 force; // force that the emitter uses to fire particles
	int particleCount = 0; // number of particles the emitter has
	float emitSpeed = 1.0f; // the speed at which particles emit

	// Constructor for the emitter
	ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const int columns, const int rows);

	// Update the particles
	virtual void update(const float delta_time);
	//Render the particles
	void render(const mat4& PV);
	// awaken particle p
	void awakenParticle(Particle& p);
	// Set the emitter's position
	void setPosition(const vec3& v);
	// set the emitter's colour
	void setColour(const vec4& v);
	
	// Delete the emitter
	~ParticleEmitter(){}
};

