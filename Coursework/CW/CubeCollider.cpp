/*

Grant Smith (40111906)

TODO

*/

#include "CubeCollider.h"
#include "SphereCollider.h"

CubeCollider::CubeCollider(vec3 position, vec3 dimensions, ColliderTypes type) : Collider(position, type){
	if (type != ColliderTypes::CUBE || type != ColliderTypes::OBBCUBE)
		type = ColliderTypes::CUBE;
	this->dimensions = dimensions;

	vec3 halfDimen = dimensions*0.5f;
}

void CubeCollider::intersects(Collider& other, const vec3& velocity, IntersectionData& data){
	data.reset();
	switch (other.colliderType){
	case ColliderTypes::SPHERE: {
		sphereToCubeCollision((SphereCollider&)(other), velocity, data);
		data.amount *= -1.0f;
		break;
	}

	case ColliderTypes::CUBE:
	{

		if (colliderType == ColliderTypes::OBBCUBE){
			obbCollision((CubeCollider&)(other), velocity, data);
			data.direction *= -1.0f;
			data.amount *= -1.0f;

		}
		else {

			vec3 otherPos = other.position;
			vec3 otherDimen = ((CubeCollider&)other).dimensions;

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
		obbCollision(((CubeCollider&)other), velocity, data);
		//data.direction *= -1.0f;
		data.amount *= -1.0f;
		break;
	}
	}

}

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

void CubeCollider::obbCollision(CubeCollider& cube, const vec3& velocity, IntersectionData& data){
	data.reset();
	vec3 offset = cube.position - position;
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

	oBBCollision(cube, norms, data);
	if (dot(data.direction, offset) < 0.0f) data.direction *= -1.0f;
	data.amount = magnitude(velocity);
	data.intersection = position + data.direction * dimensions;

}

void CubeCollider::sphereToCubeCollision(SphereCollider& sphere, vec3 velocity, IntersectionData& data){
	vec3 pt = vec3(0, 0, 0);
	data.doesIntersect = testSphereObb(sphere, pt);
	data.direction = closestCollidingNormal(sphere);
	data.amount = magnitude(velocity);
}


bool CubeCollider::testSphereObb(SphereCollider& sphere, vec3 pt){
	pt = closestPtOnOBB(sphere.position);
	vec3 v = pt - sphere.position;

	return dot(v, v) <= sphere.radius * sphere.radius;
}

vec3 CubeCollider::closestPtOnOBB(vec3 v){

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

void CubeCollider::oBBCollision(CubeCollider& other, const vector<vec3*>& norms, IntersectionData& data){
	data.reset();
	vec3 offset = position - other.position;

	if (magnitude(offset) > magnitude(getCorners(2.0f)[0] - position) + magnitude(other.getCorners(2.0f)[0] - other.position)) return;

	vector<float> depth = {};
	vector<vec3> axis = {};

	for (int i = 0; i < norms.size(); i++){
		if (!isZeroVec3(*norms[i])){
			pair<bool, float> out = checkProjectedIntersection(getCorners(2.0f), other.getCorners(2.0f), *norms[i], offset);

			if (!out.first) return;
			axis.push_back(*norms[i]);
			depth.push_back(out.second);
		}
	}

	int minPos = 0;
	float minVal = std::numeric_limits<float>::max();
	for (int i = 0; i < (int) axis.size(); i++){
		if (abs(depth.at(i)) < minVal){ minVal = depth.at(i); minPos = i; }
	}

	data.doesIntersect = true;
	data.amount = depth[minPos];
	data.direction = normalize(axis[minPos]);
}

pair<bool, float> CubeCollider::checkProjectedIntersection(vector<vec3>& corners0, vector<vec3>& corners1, const vec3& axis, const vec3& offset){
	float min0 = std::numeric_limits<float>::max();
	float max0 = std::numeric_limits<float>::min();
	float min1 = std::numeric_limits<float>::max();
	float max1 = std::numeric_limits<float>::min();

	if (isZeroVec3(axis))
		return pair<bool, float>(true, std::numeric_limits<float>::max());

	// Calc min and max intervals of each Collider
	for (int i = 0; i < 7; i++) {
		float aDist = dot(corners0[i], axis);
		if (aDist < min0) min0 = aDist;
		if (aDist > max0) max0 = aDist;
		float bDist = dot(corners1[i], axis);
		if (bDist < min1) min1 = bDist;
		if (bDist > max1) max1 = bDist;
	}

	float displace = dot(offset, axis);
	min0 += displace;
	max0 += displace;

	// 1D intersection test between 0 and 1
	float longSpan = fmax(max0, max1) - fmin(min0, min1);
	float sumSpan = max0 - min0 + max1 - min1;

	return pair<bool, float>((longSpan <= sumSpan), (sumSpan - longSpan));
}

void CubeCollider::setRotation(vec3 axis, float degrees){
	if (colliderType == ColliderTypes::OBBCUBE){
		rotation = Util::rotationMat4(axis, degrees);
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

vector<vec3> CubeCollider::getCorners(const float& scale){
	vec3 halfDimen = dimensions * scale;
	//TODO Add in the rotation code here? if(colliderType == ColliderTypes.OBBCUBE)
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

IntersectionData CubeCollider::rayCast(vec3& start, vec3& direction){
	return rayCast(start, direction, std::numeric_limits<float>::max());
}

IntersectionData CubeCollider::rayCast(vec3& start, vec3& direction, float distance){
	IntersectionData data = IntersectionData();

	data.amount = distance;

	vec3 delta = normalize(position - start);

	float tMin = 0.0f;
	float tMax = distance;

	vec3 minPoint = -dimensions * 0.25f;
	vec3 maxPoint = dimensions * 0.25f;

	for (int i = 0; i < normals.size(); i++){

		float e = dot(normals[i], delta);
		float f = dot(direction, normals[i]);

		if (fabs(f) > 0.0f){
			// Beware, don't do the division if f is near 0 ! See full source code for details. 
			float t1 = (e + minPoint[i]) / f; // Intersection with the "left" plane
			float t2 = (e + maxPoint[i]) / f; // Intersection with the "right" plane

			if (t1 > t2){ // if wrong order
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}
			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax) tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin) tMin = t1;

			if (tMax < tMin)
				return data;
		}
		else{
			if (-e + minPoint[i] > 0.0f || -e + maxPoint[i] < 0.0f)
				return data;
		}
	}
	data.intersection = position + (direction*-tMax);
	data.amount = magnitude(data.intersection - start);
	data.direction = direction;
	data.doesIntersect = true;
	return data;
}

float CubeCollider::sqdDistPointAABB(vec3 p, CubeCollider aabb){
	float sq = 0.0;
	sq += sqdValue(p.x, aabb.getMin().x, aabb.getMax().x);
	sq += sqdValue(p.y, aabb.getMin().y, aabb.getMax().y);
	sq += sqdValue(p.z, aabb.getMin().z, aabb.getMax().z);
	return sq;
}

float CubeCollider::sqdValue(float pVal, float bmin, float bmax){
	float out = 0.0;

	if (pVal < bmin){
		float value = (bmin - pVal);
		out += value * value;
	}

	if (pVal > bmax){
		float value = (pVal - bmax);
		out += value * value;
	}
	return out;
}

// Returns AABB Min
vec3 CubeCollider::getMin(){
	return position - dimensions * 0.5f;
}

// Returns AABB Max
vec3 CubeCollider::getMax(){
	return position + dimensions * 0.5f;
}

CubeCollider::~CubeCollider()
{
}