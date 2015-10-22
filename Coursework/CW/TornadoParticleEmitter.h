#pragma once

#include <renderer.h>
#include "Util.h"
#include "ParticleEmitter.h"


class TornadoParticleEmitter : public ParticleEmitter
{
public:
	TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime);

	void update(const float delta_time);

	~TornadoParticleEmitter();
};
