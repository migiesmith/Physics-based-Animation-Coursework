#pragma once

#include "Particle.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class ParticleEmitter : public vec3
{
protected:
	vector<Particle*> particles;
	float lifeTime = 5.0f;
	float emitTimer = 0.5f;

public:

	vec3 force;
	int particleCount = 0;
	float emitSpeed = 100.0f;

	ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime);

	void update(const float delta_time);
	void render();
	void awakenParticle(Particle* p);
	void setPosition(const vec3& v);
	
	~ParticleEmitter();
};

