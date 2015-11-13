#include "TornadoParticleEmitter.h"


TornadoParticleEmitter::TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const string texturePath, const int columns, const int rows) : ParticleEmitter(v, particleCount, force, lifeTime, texturePath, columns, rows)
{
}

void TornadoParticleEmitter::update(const float delta_time, const map<string, SceneObject>& sceneObjects){
	IntersectionData& data = IntersectionData();
	for (Particle& p : particles){
		if (p.isAlive){
			for (auto& mapObj : sceneObjects){
				SceneObject& sO = (SceneObject&)mapObj.second;
				//TODO make intersects take in a IntersectionData to modify
				data.reset();
				sO.intersects(*p.collider, p.velocity, data);
				if (data.doesIntersect){
					p.addForce(data.direction * data.amount * dot(data.direction, p.velocity));
				}
			}
			if (!Util::isZeroVec3(*this - p)){
				vec3 offset = normalize(*this - p);
				vec3 dir = -cross(offset, normalize(force));
				//p.addForce(force + dir*4.0f);
			}
			p.update(delta_time);
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
