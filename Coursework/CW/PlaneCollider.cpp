/*

Grant Smith (40111906)

TODO

*/

#include "PlaneCollider.h"

IntersectionData PlaneCollider::intersects(Collider* other, vec3 velocity){
	IntersectionData data = IntersectionData();

	switch (other->colliderType){
	case ColliderTypes::SPHERE:
		{
			SphereCollider* otherSphere = (SphereCollider*)other;

			// Calculate the distance the plane is from the origin
			float d0 = dot(normal, position);

			// Calculate the distance between the plane and the sphere
			float d = dot(normal, (otherSphere->position - position)) - (d0 + otherSphere->radius);

			// Set the intersection amount to d
			data.amount = d;
			// If the intersection amount is less than or equal to 0 then there is an intersection
			data.doesIntersect = data.amount <= 0;

			// Set the point of intersection to the closest point between the plane and the sphere
			data.intersection = otherSphere->position - normal*(data.amount + otherSphere->radius);

			break;
		}
	}

	return data;
}

PlaneCollider::~PlaneCollider()
{
}
