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
	vector<Particle> particles;
	float lifeTime = 5.0f;
	float emitTimer = 0.5f;
	effect particleEffect;
	texture tex;
	int columns, rows;
	vec4 colour = vec4(1, 1, 1, 1);

public:

	vec3 force;
	int particleCount = 0;
	float emitSpeed = 100.0f;

	ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const string texturePath, const int columns, const int rows);

	virtual void update(const float delta_time);
	void render(const mat4& PV);
	void awakenParticle(Particle& p);
	void setPosition(const vec3& v);
	void setColour(const vec4& v);
	
	~ParticleEmitter();
};

