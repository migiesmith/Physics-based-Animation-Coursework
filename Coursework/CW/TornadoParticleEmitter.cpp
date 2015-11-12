#include "TornadoParticleEmitter.h"


TornadoParticleEmitter::TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const string texturePath, const int columns, const int rows) : ParticleEmitter(v, particleCount, force, lifeTime, texturePath, columns, rows)
{
}

void TornadoParticleEmitter::update(const float delta_time, const map<string, SceneObject>& sceneObjects){
	for (Particle* p : particles){
		if (p->isAlive){
			if (!Util::isZeroVec3(*this - *p)){
				vec3 offset = normalize(*this - *p);
				vec3 dir = -cross(offset, normalize(force));
				p->addForce(force + dir*4.0f);
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
