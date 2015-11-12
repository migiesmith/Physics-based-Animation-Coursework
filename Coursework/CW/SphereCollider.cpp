/*

Grant Smith (40111906) - 9/11/2015

The collider that handles sphere related intersection (sphere-sphere, sphere-plane, sphere-cube, etc.)

*/

#include "SphereCollider.h"

IntersectionData SphereCollider::intersects(Collider* other, vec3 velocity){
	IntersectionData data = IntersectionData();

	switch(other->getType()){
		case ColliderTypes::SPHERE : {
			data.direction = this->position - other->position;
			data.amount = Util::magnitude(data.direction) - (this->radius + dynamic_cast<SphereCollider*>(other)->radius);
			data.direction = normalize(data.direction);
			if (data.amount <= 0){
				data.doesIntersect = true;
				data.intersection = position + (radius * normalize(-data.direction));
			}
			break;
			
			}
			
		case ColliderTypes::CUBE :
		{
			data = dynamic_cast<CubeCollider*>(other)->intersects(this, velocity);
			data.direction = data.direction * -1.0f;
			data.intersection = position + (radius * normalize(data.direction));
			break;
		}
		case ColliderTypes::OBBCUBE:
		{
			data = dynamic_cast<CubeCollider*>(other)->intersects(this, velocity);
			data.direction = data.direction * -1.0f;
			data.intersection = position + (radius *normalize(data.direction));
			break;
		}
		case ColliderTypes::PLANE:
		{
			data = dynamic_cast<PlaneCollider*>(other)->intersects(this, velocity);
			break;
		}
	}

	return data;
}




SphereCollider::~SphereCollider()
{
}
