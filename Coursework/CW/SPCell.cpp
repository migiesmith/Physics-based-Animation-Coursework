#include "SPCell.h"


SPCell::SPCell()
{
}

void SPCell::intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data){
	for (Collider* c : _colliders){
		if (c != &inCollider)
			c->intersects(inCollider, velocity, data);
		if (data.doesIntersect) return;
	}
}

SPCell::~SPCell()
{
}
