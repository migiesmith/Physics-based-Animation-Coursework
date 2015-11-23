/*
Grant Smith (40111906)

Manages the emitting of particles but as if they are part of a tornado

*/

#pragma once

#include <renderer.h>
#include "Util.h"
#include "ParticleEmitter.h"


class TornadoParticleEmitter : public ParticleEmitter
{
public:

	// Constructor for the emitter
	TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const int columns, const int rows);

	// Update the particles
	void update(const float delta_time) override;

	// Delete the emitter
	~TornadoParticleEmitter(){}
};

