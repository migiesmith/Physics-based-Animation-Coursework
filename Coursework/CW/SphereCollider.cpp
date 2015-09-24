/*

Grant Smith (40111906)

TODO

*/

#include "SphereCollider.h"

IntersectionData SphereCollider::intersects(Collider* other, float velocity){
	IntersectionData data = IntersectionData();

	switch(other->getType()){
		case ColliderTypes::SPHERE : {
			data.direction = this->position - other->position;
			data.amount = Util::magnitude(data.direction) - (this->radius + dynamic_cast<SphereCollider*>(other)->radius);
			if (data.amount <= 0){
				data.doesIntersect = true;
			}
			break;
			
			}
			
		case ColliderTypes::CUBE :
		{
			data = dynamic_cast<CubeCollider*>(other)->intersects(this, velocity);
			data.direction = data.direction * -1.0f;
			break;
		}
		case ColliderTypes::OBBCUBE:
		{
			data = dynamic_cast<CubeCollider*>(other)->intersects(this, velocity);
			data.direction = data.direction * -1.0f;
			break;
		}
	}

	return data;
}

SphereCollider::~SphereCollider()
{
}
