#include "Particle.h"

// Particle constructor
Particle::Particle(const vec3 v, const float mass) : Particle(v.x, v.y, v.z, mass)
{

}

// Particle constructor
Particle::Particle(const float x, const float y, const float z, const float mass) : vec3(x, y, z)
{
	collider = new SphereCollider(vec3(x, y, z), 0.2f);
	isAlive = true;
	setMass(mass);
}

// Apply a force
void Particle::addForce(const vec3& f){
	collider->addForce(f);
}

// Awaken the particle for updating and rendering
void Particle::awaken(const vec3& v, const vec3& force, const float lifeTime){
	setLifeTime(lifeTime);
	setPosition(v);
	collider->velocity = vec3(0, 0, 0);
	addForce(force);
}

// Integrate the velocity and position
void Particle::update(const float delta_time){
	// If this particle isn't alive then return
	if (!isAlive) return;

	// Only apply gravity if needed
	if(!collider->ignoreGravity)
		addForce(vec3(0, -9.8, 0));

	// Calculate the acceleration
	vec3 acceleration = collider->force * collider->invMass;
	// Calculate the velocity
	collider->velocity += acceleration * delta_time;
	// Add the velocity over delta time to the position
	setPosition(*this + (collider->velocity * delta_time));
	collider->setPosition(*this);

	// Reset the force and dampen the acceleration and velocity
	collider->force = vec3(0, 0, 0);
	acceleration *= 0.9f;
	collider->velocity *= 0.999f;

	lifeTime -= delta_time;
	if (lifeTime <= 0.0) isAlive = false;
}

// set the particles life time
void Particle::setLifeTime(const float t){
	lifeTime = t - fmod((float)rand(), t*0.1f) / 100.0f;
	isAlive = true;
}

// set the particles position
void Particle::setPosition(const vec3& v){
	x = v.x;
	y = v.y;
	z = v.z;
}

// Set the mass of the particle
void Particle::setMass(const float m){
	collider->setMass(m);
}
