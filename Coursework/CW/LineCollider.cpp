#include "LineCollider.h"


IntersectionData LineCollider::intersects(Collider* other, vec3 velocity){
	IntersectionData data = IntersectionData();

	switch (other->colliderType){
	case ColliderTypes::LINE:
		LineCollider* otherLine = (LineCollider*)other;

		data.doesIntersect = true;
		
		vec3 dirA = position - endPosition;
		vec3 dirB = otherLine->position - otherLine->endPosition;

		if (abs(dot(dirA, dirB)) == 1.0f){

			if (magnitude(position - otherLine->position) > radius + otherLine->radius)
				data.doesIntersect = false;
			
		}
		else{
			vec3 aToB = position - otherLine->position;

			float h = magnitude(cross(dirB, aToB));
			float k = magnitude(cross(dirB, dirA));

			vec3 toIntersection = h / k * dirA;

			if (h == 0.0f || k == 0.0f)
				data.doesIntersect = false;


			vec3 intersectionPoint = position - toIntersection;

			vec3 normal = normalize(position - intersectionPoint);

			float intersectAmount = abs(dot(normal, position - intersectionPoint) - dot(normal, position));

			data.intersection = intersectionPoint;
			data.amount = intersectAmount;

			if (abs(dot(normal, position - intersectionPoint) - dot(normal, position)) <= radius + otherLine->radius)
				data.doesIntersect = true;

		}
		

		break;
	}

	return data;
}

LineCollider::~LineCollider()
{
}
