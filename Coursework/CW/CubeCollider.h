/*

Grant Smith (40111906)

Handles cube intersection within the scene

*/

#pragma once
#include "Collider.h"
#include "IntersectionData.h"

using namespace Util;
class SphereCollider;

class CubeCollider :
	public Collider
{

public:

	// Dimensions of the cube
	vec3 dimensions;

	// The rotation of the cube as a matrix
	mat4 rotation = mat4(1.0f);

	// The cube's normals
	vector<vec3> normals = 
	vector<vec3>{
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	};

	// Cube collider constructor
	CubeCollider(vec3 position, vec3 dimensions, ColliderTypes type);

	// Check if this collider collides with other
	void intersects(Collider& other, const vec3& velocity, IntersectionData& data);
	// Run the 1D intersection test (used within the obb collision check)
	pair<bool, float> checkProjectedIntersection(vector<vec3>& corners0, vector<vec3>& corners1, const vec3& axis, const vec3& offset);
	// Get the corners of the cube
	vector<vec3> getCorners(const float& scale);
	// Set the rotation of the (OBB only)
	void setRotation(const mat4& rotation);

	// Find the closest colliding normal between this and the other collider
	vec3 closestCollidingNormal(Collider& other);
	// Start running the obb collision check (mostly set up)
	void runOBBCollision(CubeCollider& cube, const vec3& velocity, IntersectionData& data);
	// Run the bulk of the obb collision check
	void checkOBBCollisionWithNormals(CubeCollider& other, const vector<vec3*>& norms, IntersectionData& data);
	// Run the sphere v cube collision check
	void sphereToCubeCollision(SphereCollider& sphere, const vec3& velocity, IntersectionData& data);
	// Run the sphere v obb collision check
	bool sphereVObbCollision(SphereCollider& sphere, const vec3& pt);
	// Get the closest point on the obb to point v
	vec3 closestPtOnOBB(const vec3& v);

	// Returns AABB Min
	vec3 getMin();
	// Returns AABB Max
	vec3 getMax();

	// Delete the cube collider
	~CubeCollider(){}
};

