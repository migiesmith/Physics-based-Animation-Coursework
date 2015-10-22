#include "TornadoParticleEmitter.h"


TornadoParticleEmitter::TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime) : ParticleEmitter(v, particleCount, force, lifeTime)
{
}

void TornadoParticleEmitter::update(const float delta_time){
	for (Particle* p : particles){
		if (p->isAlive){
			if (!Util::isZeroVec3(*this - *p)){
				vec3 offset = normalize(*this - *p);
				vec3 dir = cross(offset, normalize(force));
				offset.y = 0;
				offset = normalize(offset);
				p->addForce(force + dir);
			}
			p->update(delta_time);
		}
		else if (emitTimer <= 0.0f){
			awakenParticle(p);
		}
	}
	emitTimer -= delta_time;
}

TornadoParticleEmitter::~TornadoParticleEmitter()
{
}
