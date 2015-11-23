#include "TornadoParticleEmitter.h"


// Constructor for the emitter
TornadoParticleEmitter::TornadoParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const int columns, const int rows) : ParticleEmitter(v, particleCount, force, lifeTime, columns, rows)
{
	for (Particle& p : particles){
		p.collider->ignoreGravity = true;
	}
}

// Update the particles
void TornadoParticleEmitter::update(const float delta_time){
	IntersectionData& data = IntersectionData();
	SPGrid& spGrid = SPGrid::getInstance();

	for (Particle& p : particles){
		if (p.isAlive){

			data.reset();
			p.update(delta_time);
			spGrid.intersects(*p.collider, p.collider->velocity, data);

			// If the particle isn't right on the emitter
			if (!Util::isZeroVec3(*this - p)){
				// Apply a force to rotate it around the emitter and upwards
				vec3 offset = normalize(*this - p);
				vec3 dir = -normalize(normalize(cross(offset, normalize(force))) - normalize(offset)*4.0f);
				p.addForce(force + dir*magnitude(force));
			}
			p.update(delta_time);
		}
		else if (emitTimer <= 0.0f){
			awakenParticle(p);
		}
	}
	emitTimer -= delta_time;
}