#include "Particle.h"

Particle::Particle(const vec3 v, const float mass) : Particle(v.x, v.y, v.z, mass)
{

}

Particle::Particle(const float x, const float y, const float z, const float mass) : vec3(x, y, z)
{
	velocity = vec3(0, 0, 0); 
	force = vec3(0, 0, 0);
	isAlive = true;
	setMass(mass);
	collider = new SphereCollider(vec3(x, y, z), 0.1f);
}

void Particle::addForce(const vec3& f){
	force += f;
}

void Particle::awaken(const vec3& v, const vec3& force, const float lifeTime){
	setLifeTime(lifeTime);
	setPosition(v);
	velocity = vec3(0, 0, 0);
	addForce(force);
}

void Particle::update(const float delta_time){
	if (!isAlive) return;

	addForce(vec3(0, -9.8, 0));

	vec3 acceleration = force * invMass;
	velocity += acceleration * delta_time;
	setPosition(*this + (velocity * delta_time));
	collider->setPosition(*this);

	force = vec3(0,0,0);
	acceleration *= 0.9f;

	lifeTime -= delta_time;
	if (lifeTime <= 0.0) isAlive = false;
}


void Particle::render(){
	if (!isAlive) return;
	glBegin(GL_POINTS);
	glVertex3f(x,y,z);
	glEnd();
}

void Particle::setLifeTime(const float t){
	lifeTime = t - fmod(rand(), t*0.1f) / 100.0f;
	isAlive = true;
}

void Particle::setPosition(const vec3& v){
	x = v.x;
	y = v.y;
	z = v.z;
}

void Particle::setMass(const float m){
	invMass = 1.0f / m;
}

Particle::~Particle()
{

}
