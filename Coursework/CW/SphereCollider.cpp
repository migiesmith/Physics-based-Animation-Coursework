#include "SphereCollider.h"
#include <math.h>

IntersectionData SphereCollider::intersects(Collider other){
	IntersectionData data = IntersectionData();

	switch(other.getType()){
		case ColliderTypes::SPHERE : {
				data.direction = this->position - other.position;
				float dist = sqrtf(pow(data.direction.x, 2) + pow(data.direction.y, 2) + pow(data.direction.z, 2));
				//TODO use util
				cout << "SPHERE | " << dist << endl;
				if (dist < this->radius + static_cast<SphereCollider&>(other).radius)
					data.doesIntersect = true;

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
