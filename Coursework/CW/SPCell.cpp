#include "SPCell.h"


SPCell::SPCell()
{
}

void SPCell::intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data){
	for (Collider* c : _colliders){
		if (c != &inCollider){
			data.reset();
			c->intersects(inCollider, velocity, data);

			vec3 correctiveVelocity = data.direction * dot(data.direction, (inCollider.velocity - c->velocity));

			if (data.doesIntersect){
				if (c->staticPos){
					inCollider.velocity -= (correctiveVelocity);
				}
				else{
					inCollider.velocity -= (correctiveVelocity * (inCollider.invMass / c->invMass));
					c->velocity -= -(correctiveVelocity* (c->invMass / inCollider.invMass));
				}
				
				inCollider.addForce(data.direction * dot(data.direction, inCollider.velocity));
				inCollider.addForce(vec3(0, 9.8, 0) * dot(vec3(0, 1, 0), data.direction)*2.0f);

			}

			testCount++;
		}
	}
	beenChecked = true;
}

SPCell::~SPCell()
{
}