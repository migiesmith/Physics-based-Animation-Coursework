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

bool Collider::isSolid(){
	return _solid;
}

Collider::~Collider()
{
}
