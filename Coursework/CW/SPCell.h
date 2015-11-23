/*

Grant Smith (40111906) - 23/11/2015

An individual cell of the SPGrid
Handles collider intersection and stores a list of colliders within this cell

*/
#pragma once

#include "CubeCollider.h"
#include "Util.h"

class SPCell
{
private:

	// A list containing pointers to each collider this cell contains
	vector<Collider*> _colliders;

public:
	
	int testCount = 0; // number of tests that have ran in the last updated
	bool beenChecked = false; // if the cell was checked last update
	
	// Default constructor for cell
	SPCell(){}
	
	// add collider c to the list if it isn't already there
	void addCollider(Collider* c){
		if (!contains(c))_colliders.push_back(c);
	}

	// Check if inCollider intersects and collider in this cell and resolve the collision
	void intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data);

	// Check if this cell already contains collider c
	bool contains(Collider* c){ return std::find(_colliders.begin(), _colliders.end(), c) != _colliders.end(); }
	// clear the list of pointers to colliders
	void clear(){ _colliders.clear(); testCount = 0; beenChecked = false; }
	int colliderCount(){ return _colliders.size(); }

	// Delete the cell
	~SPCell(){}
};

