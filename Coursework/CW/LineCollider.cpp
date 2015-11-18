#include "LineCollider.h"


void LineCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){
	data.reset();

	switch (other.colliderType){
	case ColliderTypes::LINE:
		LineCollider& otherLine = (LineCollider&)other;

		data.doesIntersect = true;
		
		vec3 dirA = position - endPosition;
		vec3 dirB = otherLine.position - otherLine.endPosition;

		if (abs(dot(dirA, dirB)) == 1.0f){

			if (magnitude(position - otherLine.position) > radius + otherLine.radius){
				data.doesIntersect = false;
				return;
			}
			
		}
		else{
			vec3 aToB = position - otherLine.position;

			float h = magnitude(cross(dirB, aToB));
			float k = magnitude(cross(dirB, dirA));

			vec3 toIntersection = h / k * dirA;

			if (h == 0.0f || k == 0.0f){
				data.doesIntersect = false;
				return;
			}


			vec3 intersectionPoint = position - toIntersection;

			vec3 normal = normalize(position - intersectionPoint);

			float intersectAmount = abs(dot(normal, position - intersectionPoint) - dot(normal, position));

			data.intersection = intersectionPoint;
			data.amount = intersectAmount;

			if (abs(dot(normal, position - intersectionPoint) - dot(normal, position)) <= radius + otherLine.radius)
				data.doesIntersect = true;

			data.direction = aToB;
		}
		

		break;
	}
}

LineCollider::~LineCollider()
{
}
