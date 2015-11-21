/*

Grant Smith (40111906)

TODO

*/

#include "PlaneCollider.h"

void PlaneCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){
	data.reset();
	
	switch (other.colliderType){
	case ColliderTypes::SPHERE:
		{
			SphereCollider& otherSphere = (SphereCollider&)other;

			// Calculate the distance the plane is from the origin
			float d0 = dot(normal, position);

			// Calculate the distance between the plane and the sphere
			float d = dot(normal, (otherSphere.position - position)) - (d0 + otherSphere.radius);

			// Set the intersection amount to d
			data.amount = d;
			// If the intersection amount is less than or equal to 0 then there is an intersection
			data.doesIntersect = data.amount <= 0;

			// Set the point of intersection to the closest point between the plane and the sphere
			data.intersection = otherSphere.position - normal*(data.amount + otherSphere.radius);

			data.direction = normal;

			break;
		}
		case ColliderTypes::CUBE:
		{
			cubeIntersection((CubeCollider&)other, velocity, data);
			break;
		}
		case ColliderTypes::OBBCUBE:
		{
			cubeIntersection((CubeCollider&)other, velocity, data);
			break;
		}
	}
}

vec3 PlaneCollider::rayIntersection(vec3& start, vec3& direction){
	vec3 P;
	
	float d = -dot(position, normal);
	float t = -(dot(start, normal + d)) / dot(direction, normal);

	P = start + t * direction;

	return P;
}

void PlaneCollider::cubeIntersection(CubeCollider& other, const vec3& velocity, IntersectionData& data){


	// Calculate the distance the plane is from the origin
	float d0 = dot(normal, position);

	vector<vec3>& corners = other.getCorners(1.0f);

	float minDist = numeric_limits<float>().max();
	for (vec3& corner : corners){
		// Calculate the distance between the plane and the sphere
		float d = dot(normal, (corner - position)) - d0;

		if (d <= minDist){
			minDist = d;

			// Set the point of intersection to the closest colliding corner
			data.intersection = corner;
		}
	}

	// Set the intersection amount to d
	data.amount = minDist;
	// If the intersection amount is less than or equal to 0 then there is an intersection
	data.doesIntersect = data.amount <= 0;
	
	data.direction = normal;

}

PlaneCollider::~PlaneCollider()
{
}
