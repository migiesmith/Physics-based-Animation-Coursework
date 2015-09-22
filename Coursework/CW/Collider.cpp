/*

Grant Smith (40111906)

Constructor/Destructor of the SceneObject

*/

#include "Collider.h"

Collider::Collider(vec3 position, ColliderTypes colliderType)
{
	this->position = position;
	this->colliderType = colliderType;
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

Collider::~Collider()
{
}
