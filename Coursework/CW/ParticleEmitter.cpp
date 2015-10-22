#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime) : vec3(v)
{
	this->force = force;
	this->lifeTime = lifeTime;
	this->particleCount = particleCount;
	for (int i = 0; i < particleCount; i++){
		Particle* p = new Particle(v, 1.0f);
		p->isAlive = false;
		particles.push_back(p);
	}
}

void ParticleEmitter::update(const float delta_time){
	for (Particle* p : particles){
		if (p->isAlive){
			p->update(delta_time);
		}else if(emitTimer <= 0.0f){
			awakenParticle(p);
		}
	}

	emitTimer -= delta_time;
}

void ParticleEmitter::awakenParticle(Particle* p){
	vec3 offset = normalize(force);

	for (int i = 0; i < 3; i++){
		offset[i] = ((((rand() % 200) - 100.0f) / 100.0f) * (1.0f - offset[i])) * 120.0f;
	}

	p->awaken(*this, force + offset, lifeTime);
	emitTimer = 1.0f / emitSpeed;
}

void ParticleEmitter::render(){
	for (Particle* p : particles){
		p->render();
	}
}

void ParticleEmitter::setPosition(const vec3& v){
	x = v.x;
	y = v.y;
	z = v.z;
}

ParticleEmitter::~ParticleEmitter()
{
	for (Particle* p : particles)
		delete p;
}
