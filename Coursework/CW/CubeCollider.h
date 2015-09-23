/*

Grant Smith (40111906)

TODO

*/

#pragma once
#include "SphereCollider.h"
#include "IntersectionData.h"
#include "Util.h"

using namespace Util;

class CubeCollider :
	public Collider
{
public:

	vec3 dimensions;

	mat4 rotation = mat4();

	vector<vec3> normals = 
	vector<vec3>{
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	};

	CubeCollider(vec3 position, vec3 dimensions, ColliderTypes type) : Collider(position, type){
		if (type != ColliderTypes::CUBE || type != ColliderTypes::OBBCUBE)
			type = ColliderTypes::CUBE;
		this->dimensions = dimensions;
	}

	IntersectionData intersects(Collider* other);
	IntersectionData oBBCollision(CubeCollider other, vector<vec3> norms);
	pair<bool, float> checkProjectedIntersection(vector<vec3> corners0, vector<vec3> corners1, vec3 axis, vec3 offset);
	vector<vec3> getCorners();
	void rotate(vec3 axis, float degrees);
	float sqdDistPointAABB(vec3 p, CubeCollider aabb);
	float sqdValue(float pVal, float bmin, float bmax);

	// Returns AABB Min
	vec3 getMin();
	// Returns AABB Max
	vec3 getMax();

	~CubeCollider();
};

