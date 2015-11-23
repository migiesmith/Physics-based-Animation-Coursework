#include "LineCollider.h"


// check for an intersection between this line and another collider
void LineCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){
	data.reset();

	switch (other.colliderType){
	case ColliderTypes::LINE:
		LineCollider& otherLine = (LineCollider&)other;

		data.doesIntersect = true;
		
		// Get the direction of this line and the other
		vec3 dirA = position - endPosition;
		vec3 dirB = otherLine.position - otherLine.endPosition;

		if (abs(dot(dirA, dirB)) == 1.0f){

			// if they are parallel, no collision
			if (magnitude(position - otherLine.position) > radius + otherLine.radius){
				data.doesIntersect = false;
				return;
			}
			
		}
		else{
			// get the direction from this line to the other
			vec3 aToB = position - otherLine.position;

			float h = magnitude(cross(dirB, aToB));
			float k = magnitude(cross(dirB, dirA));

			if (h == 0.0f || k == 0.0f){
				data.doesIntersect = false;
				return;
			}

			// calculate the distance to the intersection
			vec3 toIntersection = h / k * dirA;
			// Get the intersection point
			vec3 intersectionPoint = position - toIntersection;

			// get the direction from this line's start to the intersection point
			vec3 normal = normalize(position - intersectionPoint);

			// calculate the amount of intersection
			float intersectAmount = abs(dot(normal, intersectionPoint) - dot(normal, position));


			data.intersection = intersectionPoint;
			data.amount = intersectAmount;

			// If the amount of intersection is less than or equal to the sum of each line's radius then they intersect
			if (intersectAmount <= radius + otherLine.radius)
				data.doesIntersect = true;

			data.direction = aToB;

		}
		

		break;
	}
}