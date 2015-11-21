// Spatial Partitioning Cell

#pragma once

#include "CubeCollider.h"
#include "Util.h"

class SPCell
{
private:

	vector<Collider*> _colliders;

public:
	
	int testCount = 0;
	bool beenChecked = false;
	
	SPCell();
	
	void addCollider(Collider* c){
		if (!contains(c))_colliders.push_back(c);
	}

	void intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data);

	bool contains(Collider* c){ return std::find(_colliders.begin(), _colliders.end(), c) != _colliders.end(); }
	void clear(){ _colliders.clear(); testCount = 0; beenChecked = false; }
	int colliderCount(){ return _colliders.size(); }

	~SPCell();
};

