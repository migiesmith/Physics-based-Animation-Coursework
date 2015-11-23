#include "SphereCollider.h"

// Checks for collision between the sphere and collider other
void  SphereCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){

	switch(other.getType()){
		case ColliderTypes::SPHERE : {
			data.direction = this->position - other.position;
			data.amount = Util::magnitude(data.direction) - (this->radius + ((SphereCollider&)other).radius);
			data.direction = normalize(data.direction);
			if (data.amount <= 0){
				data.doesIntersect = true;
				data.intersection = position + (radius * normalize(-data.direction));
			}
			break;
			
			}
			
		case ColliderTypes::CUBE :
		{
			other.intersects(*this, velocity, data);
			data.intersection = position + (radius * normalize(data.direction));
			break;
		}
		case ColliderTypes::OBBCUBE:
		{
			other.intersects(*this, velocity, data);
			data.intersection = position + (radius *normalize(data.direction));
			break;
		}
		case ColliderTypes::PLANE:
		{
			other.intersects(*this, velocity, data);
			break;
		}
	}

}