#include "CubeCollider.h"
#include "SphereCollider.h"

// Cube collider constructor
CubeCollider::CubeCollider(vec3 position, vec3 dimensions, ColliderTypes type) : Collider(position, type){
	if (type != ColliderTypes::CUBE || type != ColliderTypes::OBBCUBE)
		type = ColliderTypes::CUBE;
	this->dimensions = dimensions;

	vec3 halfDimen = dimensions*0.5f;
}

// Check if this collider collides with other
void CubeCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){
	data.reset();
	switch (other.colliderType){
	case ColliderTypes::SPHERE: {
		sphereToCubeCollision((SphereCollider&)(other), velocity, data);
		break;
	}

	case ColliderTypes::CUBE:
	{

		if (colliderType == ColliderTypes::OBBCUBE){
			runOBBCollision((CubeCollider&)(other), velocity, data);
			data.direction *= -1.0f;
			data.amount *= -1.0f;

		}
		else {

			vec3 otherPos = other.position;
			vec3 otherDimen = ((CubeCollider&)other).dimensions;
			// Check if the bounds of each AABB cube overlaps
			if (abs(position.x - otherPos.x) < dimensions.x + otherDimen.x) {
				if (abs(position.y - otherPos.y) < dimensions.y + otherDimen.y) {
					if (abs(position.z - otherPos.z) < dimensions.z + otherDimen.z) {
						data.doesIntersect = true;
						data.direction = -closestCollidingNormal(other);
						data.amount = -magnitude(velocity);
					}
				}
			}
		}
		break;
	}

	case ColliderTypes::OBBCUBE:
	{
		runOBBCollision(((CubeCollider&)other), velocity, data);
		//data.direction *= -1.0f;
		data.amount *= -1.0f;
		break;
	}
	}

}

// Find the closest colliding normal between this and the other collider
vec3 CubeCollider::closestCollidingNormal(Collider& other){
	vec3 direction = position - other.position;
	if (!isZeroVec3(direction))
		direction = normalize(direction);

	float minCollision = std::numeric_limits<float>::max();
	int closestIndex = 0;
	vector<vec3> norms = {
		normals[0],
		normals[1],
		normals[2],
		-normals[0],
		-normals[1],
		-normals[2]
	};
	for (int i = 0; i < (int) norms.size(); i++){
		float d = dot(direction, (position + (norms[i] * (dimensions / 2.0f)) - other.position));
		if (d < minCollision){
			closestIndex = i;
			minCollision = d;
		}
	}
	vec3 outNorm = norms[closestIndex];
	if (!isZeroVec3(outNorm)) outNorm = normalize(outNorm);
	return outNorm;
}

// Start running the obb collision check (mostly set up)
void CubeCollider::runOBBCollision(CubeCollider& cube, const vec3& velocity, IntersectionData& data){
	data.reset();
	vec3 offset = cube.position - position;
	// Set up the list of axis to check
	vector<vec3*> norms = {
		&normals[0],
		&normals[1],
		&normals[2],
		&cube.normals[0],
		&cube.normals[1],
		&cube.normals[2],
		&cross(normals[0], cube.normals[0]),
		&cross(normals[0], cube.normals[1]),
		&cross(normals[0], cube.normals[2]),
		&cross(normals[1], cube.normals[0]),
		&cross(normals[1], cube.normals[1]),
		&cross(normals[1], cube.normals[2]),
		&cross(normals[2], cube.normals[0]),
		&cross(normals[2], cube.normals[1]),
		&cross(normals[2], cube.normals[2]),
	};
	if (!isZeroVec3(offset)) norms.push_back(&(normalize(offset) * -1.0f));

	// Run the collision check on the norms list
	checkOBBCollisionWithNormals(cube, norms, data);
	if (dot(data.direction, offset) < 0.0f) data.direction *= -1.0f;
	data.amount = magnitude(velocity);
	data.intersection = position + data.direction * dimensions;

}

// Run the sphere v cube collision check
void CubeCollider::sphereToCubeCollision(SphereCollider& sphere, const vec3& velocity, IntersectionData& data){
	vec3 pt = vec3(0, 0, 0);
	data.doesIntersect = sphereVObbCollision(sphere, pt);
	data.direction = closestCollidingNormal(sphere);
	if (dot(data.direction, normalize(position - sphere.position)) < 0.0f) data.direction *= -1.0f;
	data.amount = magnitude(velocity);
}


// Run the sphere v obb collision check
bool CubeCollider::sphereVObbCollision(SphereCollider& sphere, const vec3& pt){
	vec3 v = closestPtOnOBB(sphere.position) - sphere.position;

	return dot(v, v) <= sphere.radius * sphere.radius;
}

// Get the closest point on the obb to point v
vec3 CubeCollider::closestPtOnOBB(const vec3& v){

	vec3 retPt;
	vec3 d = v - position;
	retPt = position;

	for (int i = 0; i < 3; ++i)
	{
		float dist = dot(d, normals[i]);

		if (dist > dimensions[i]) dist = dimensions[i];
		if (dist < -dimensions[i]) dist = -dimensions[i];
		retPt += dist * normals[i];
	}
	return retPt;

}

// Run the bulk of the obb collision check
void CubeCollider::checkOBBCollisionWithNormals(CubeCollider& other, const vector<vec3*>& norms, IntersectionData& data){
	data.reset();
	// Get the offset between this and the other collider
	vec3 offset = position - other.position;

	// if these colliders are too far apart, return
	if (magnitude(offset) > magnitude(getCorners(2.0f)[0] - position) + magnitude(other.getCorners(2.0f)[0] - other.position)) return;

	// Create lists to store the axis and depths
	vector<float> depth = {};
	vector<vec3> axis = {};

	for (int i = 0; i < (int)norms.size(); i++){
		if (!isZeroVec3(*norms[i])){
			pair<bool, float> out = checkProjectedIntersection(getCorners(2.0f), other.getCorners(2.0f), *norms[i], offset);

			if (!out.first) return;
			axis.push_back(*norms[i]);
			depth.push_back(out.second);
		}
	}

	// Pick the smallest depth
	int minPos = 0;
	float minVal = std::numeric_limits<float>::max();
	for (int i = 0; i < (int) axis.size(); i++){
		if (abs(depth.at(i)) < minVal){ minVal = depth.at(i); minPos = i; }
	}

	// Return the smallest depth and the related axis
	data.doesIntersect = true;
	data.amount = depth[minPos];
	data.direction = normalize(axis[minPos]);
}

// Run the 1D intersection test (used within the obb collision check)
pair<bool, float> CubeCollider::checkProjectedIntersection(vector<vec3>& corners0, vector<vec3>& corners1, const vec3& axis, const vec3& offset){
	// Check that axis is valid
	if (isZeroVec3(axis))
		return pair<bool, float>(true, std::numeric_limits<float>::max());

	// initialise the min and max values
	float min0 = std::numeric_limits<float>::max();
	float max0 = std::numeric_limits<float>::min();
	float min1 = std::numeric_limits<float>::max();
	float max1 = std::numeric_limits<float>::min();


	// Calc min and max intervals of each Collider
	for (int i = 0; i < 7; i++) {
		float aDist = dot(corners0[i], axis);
		if (aDist < min0) min0 = aDist;
		if (aDist > max0) max0 = aDist;
		float bDist = dot(corners1[i], axis);
		if (bDist < min1) min1 = bDist;
		if (bDist > max1) max1 = bDist;
	}

	// apply the displacement to correct the values
	float displace = dot(offset, axis);
	min0 += displace;
	max0 += displace;

	// 1D intersection test between 0 and 1
	float longSpan = fmax(max0, max1) - fmin(min0, min1);
	float sumSpan = max0 - min0 + max1 - min1;

	// return whether or not they intersect and by how much
	return pair<bool, float>((longSpan <= sumSpan), (sumSpan - longSpan));
}

// Set the rotation of the (OBB only)
void CubeCollider::setRotation(const mat4& rotation){
	if (colliderType == ColliderTypes::OBBCUBE){
		this->rotation = rotation;
		vector<vec3> newNorms{
			vec3(1.0, 0.0, 0.0),
			vec3(0.0, 1.0, 0.0),
			vec3(0.0, 0.0, 1.0)
		};
		for (int i = 0; i < 2; i++){
			vec4 rotatedNorm = vec4(newNorms[i], 1.0) * rotation;
			normals[i] = vec3(rotatedNorm.x, rotatedNorm.y, rotatedNorm.z);
		}
	}
	else{
		cout << ("Missues of CubeCollider - You cannot rotate a cube that isn't an OBBCUBE") << endl;
	}
}

// Get the corners of the cube
vector<vec3> CubeCollider::getCorners(const float& scale){
	vec3 halfDimen = dimensions * scale;

	if (colliderType == ColliderTypes::CUBE){
		return{
			position + vec3(halfDimen.x, halfDimen.y, halfDimen.z),//Vec3(1.0,1.0,1.0),
			position + vec3(-halfDimen.x, halfDimen.y, halfDimen.z),//Vec3(-1.0,1.0,1.0),
			position + vec3(-halfDimen.x, halfDimen.y, -halfDimen.z),//Vec3(-1.0,1.0,-1.0),
			position + vec3(halfDimen.x, halfDimen.y, -halfDimen.z),//Vec3(1.0,1.0,-1.0),

			position + vec3(halfDimen.x, -halfDimen.y, halfDimen.z),//Vec3(1.0,-1.0,1.0),
			position + vec3(-halfDimen.x, -halfDimen.y, halfDimen.z),//Vec3(-1.0,-1.0,1.0),
			position + vec3(-halfDimen.x, -halfDimen.y, -halfDimen.z),//Vec3(-1.0,-1.0,-1.0),
			position + vec3(halfDimen.x, -halfDimen.y, -halfDimen.z)//Vec3(1.0,-1.0,-1.0)
		};
	}
	else{
		return{
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(halfDimen.x, halfDimen.y, halfDimen.z)) * rotation),//Vec3(1.0,1.0,1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(-halfDimen.x, halfDimen.y, halfDimen.z)) * rotation),//Vec3(-1.0,1.0,1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(-halfDimen.x, halfDimen.y, -halfDimen.z)) * rotation),//Vec3(-1.0,1.0,-1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(halfDimen.x, halfDimen.y, -halfDimen.z)) * rotation),//Vec3(1.0,1.0,-1.0),

			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(halfDimen.x, -halfDimen.y, halfDimen.z)) * rotation),//Vec3(1.0,-1.0,1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(-halfDimen.x, -halfDimen.y, halfDimen.z)) * rotation),//Vec3(-1.0,-1.0,1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(-halfDimen.x, -halfDimen.y, -halfDimen.z)) * rotation),//Vec3(-1.0,-1.0,-1.0),
			position + Util::vec4ToVec3(Util::vec3ToVec4(vec3(halfDimen.x, -halfDimen.y, -halfDimen.z)) * rotation)//Vec3(1.0,-1.0,-1.0)
		};
	}
}

// Returns AABB Min
vec3 CubeCollider::getMin(){
	return position - dimensions * 0.5f;
}

// Returns AABB Max
vec3 CubeCollider::getMax(){
	return position + dimensions * 0.5f;
}