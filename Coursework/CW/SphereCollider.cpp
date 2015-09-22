/*

Grant Smith (40111906)

TODO

*/

#include "SphereCollider.h"
#include <math.h>

IntersectionData SphereCollider::intersects(Collider* other){
	IntersectionData data = IntersectionData();

	switch(other->getType()){
		case ColliderTypes::SPHERE : {
			data.direction = this->position - other->position;
			data.amount = Util::magnitude(data.direction) - (this->radius + dynamic_cast<SphereCollider*>(other)->radius);
			if (data.amount <= 0){
				data.doesIntersect = true;
			}
			
			}
			
		/*
		case ColliderTypes::CUBE :
			{
				data = ((CubeCollider)other).intersects(this);
				data.direction = data.direction*-1.0;
			}
		*/
	}

	return data;
}

SphereCollider::~SphereCollider()
{
}
