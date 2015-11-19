// Spatial Partitioning Cell

#pragma once

#include "Collider.h"
#include "Util.h"

class SPCell
{
private:

	vector<Collider*> _colliders;

public:
	
	bool beenTested = false;
	
	SPCell();
	
	void addCollider(Collider* c){
		if (!contains(c))_colliders.push_back(c);
	}

	void intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data);
	bool contains(Collider* c){ return std::find(_colliders.begin(), _colliders.end(), c) != _colliders.end(); }
	void clear(){ _colliders.clear(); beenTested = false; }
	int colliderCount(){ return _colliders.size(); }

	~SPCell();
};

