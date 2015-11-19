#include "SPCell.h"


SPCell::SPCell()
{
}

void SPCell::intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data){
	for (Collider* c : _colliders){
		if (c != &inCollider){
			data.reset();
			c->intersects(inCollider, velocity, data);
			if (data.doesIntersect){

				if (inCollider.getType() == ColliderTypes::OBBCUBE && !(abs(dot(data.direction, inCollider.velocity)) > 0.0f))
					cout << Util::vec3ToString(data.direction) << endl;

				inCollider.velocity -= (data.direction * dot(data.direction, inCollider.velocity))*1.5f;
				if (!c->staticPos)
					c->velocity -= (data.direction * (c->invMass / inCollider.invMass) * dot(data.direction, inCollider.velocity))*1.5f;

				inCollider.addForce(inCollider.force * inCollider.invMass * dot(data.direction, inCollider.velocity));
				inCollider.addForce(vec3(0, 9.8, 0) * dot(vec3(0, 1, 0), data.direction)*2.0f);
			}
		}
	}
	beenTested = true;
}

SPCell::~SPCell()
{
}