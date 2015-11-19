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
private:

	vector<vec3> _corners;

public:

	vec3 dimensions;

	mat4 rotation = mat4(1.0f);

	vector<vec3> normals = 
	vector<vec3>{
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	};

	CubeCollider(vec3 position, vec3 dimensions, ColliderTypes type);

	void intersects(Collider& other, const vec3& velocity, IntersectionData& data);
	void oBBCollision(CubeCollider& other, const vector<vec3*>& norms, IntersectionData& data);
	pair<bool, float> checkProjectedIntersection(vector<vec3>& corners0, vector<vec3>& corners1, const vec3& axis, const vec3& offset);
	vector<vec3> getCorners(const float& scale);
	void setRotation(vec3 axis, float degrees);
	float sqdDistPointAABB(vec3 p, CubeCollider aabb);
	float sqdValue(float pVal, float bmin, float bmax);

	vec3 closestCollidingNormal(Collider& other);
	void obbCollision(CubeCollider& cube, const vec3& velocity, IntersectionData& data);
	void sphereToCubeCollision(SphereCollider& sphere, vec3 velocity, IntersectionData& data);
	bool testSphereObb(SphereCollider& sphere, vec3 pt);
	vec3 closestPtOnOBB(vec3 v);

	IntersectionData rayCast(vec3& start, vec3& direction);
	IntersectionData rayCast(vec3& start, vec3& direction, float distance);

	// Returns AABB Min
	vec3 getMin();
	// Returns AABB Max
	vec3 getMax();

	~CubeCollider();
};

