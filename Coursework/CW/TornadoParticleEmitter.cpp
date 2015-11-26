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

			vec3 offset = *this - p;
			// If the particle isn't right on the emitter
			if (!Util::isZeroVec3(offset) && !Util::isZeroVec3(force)){
				// Apply a force to rotate it around the emitter and upwards
				offset = normalize(offset);
				vec3 dir = cross(offset, normalize(force));
				if (!Util::isZeroVec3(dir))
					dir = normalize(dir);
				dir -= offset*4.0f;
				if (!Util::isZeroVec3(dir))
					dir = -normalize(dir);
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