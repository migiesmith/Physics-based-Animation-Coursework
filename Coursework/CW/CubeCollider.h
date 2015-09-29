/*

Grant Smith (40111906)

TODO

*/

#pragma once
//#include "SphereCollider.h"
#include "Collider.h"
#include "IntersectionData.h"

using namespace Util;
class SphereCollider;

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

	IntersectionData intersects(Collider* other, float velocity);
	IntersectionData oBBCollision(CubeCollider other, vector<vec3> norms);
	pair<bool, float> checkProjectedIntersection(vector<vec3> corners0, vector<vec3> corners1, vec3 axis, vec3 offset);
	vector<vec3> getCorners();
	void rotate(vec3 axis, float degrees);
	float sqdDistPointAABB(vec3 p, CubeCollider aabb);
	float sqdValue(float pVal, float bmin, float bmax);

	vec3 closestCollidingNormal(Collider* other);
	IntersectionData obbCollision(CubeCollider* cube, float velocity);
	IntersectionData sphereToCubeCollision(SphereCollider* sphere, float velocity);
	bool testSphereObb(SphereCollider* sphere, CubeCollider obb, vec3 pt);
	vec3 closestPtOnOBB(vec3 v);

	// Returns AABB Min
	vec3 getMin();
	// Returns AABB Max
	vec3 getMax();

	~CubeCollider();
};
