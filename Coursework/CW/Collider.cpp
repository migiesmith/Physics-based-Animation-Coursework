/*

Grant Smith (40111906)

Constructor/Destructor of the SceneObject

*/

#include "Collider.h"

Collider::Collider(vec3 position, ColliderTypes colliderType)
{
	this->position = position;
	this->colliderType = colliderType;
	velocity = vec3(0, 0, 0);
	force = vec3(0, 0, 0);
	setMass(1.0f);
}

void Collider::setPosition(vec3 position){
	this->position = position;
}

void Collider::translate(vec3 translation){
	this->position += translation;
}

bool Collider::isSolid(){
	return _solid;
}

void Collider::addForce(const vec3& f){
	force += f;
}

// Integrate the velocity and position
void Collider::update(const float delta_time){
	// If this collider doesn't move then return
	if (staticPos)
		return;

	// Only apply gravity if needed
	if(!ignoreGravity)
		addForce(vec3(0, -9.8, 0));

	// Calculate the acceleration
	vec3 acceleration = force * invMass;
	// Calculate the velocity
	velocity += acceleration * delta_time;
	// Add the velocity over delta time to the position
	setPosition(position + (velocity * delta_time));

	// Reset the force and dampen the acceleration and velocity
	force = vec3(0, 0, 0);
	acceleration *= 0.9f;
	velocity *= 0.999f;
}

// Set the mass of the collider
void Collider::setMass(const float m){
	invMass = 1.0f / m;
}
