/*

Grant Smith (40111906)

TODO

*/

#include "CubeCollider.h"
#include "SphereCollider.h"



IntersectionData CubeCollider::intersects(Collider* other, float velocity){
	IntersectionData data = IntersectionData();

	switch(other->colliderType){
	case ColliderTypes::SPHERE : {
			data = sphereToCubeCollision(dynamic_cast<SphereCollider*>(other), velocity);
			break;
		}

	case ColliderTypes::CUBE :
		{

		if (colliderType == ColliderTypes::OBBCUBE){
			data = obbCollision(dynamic_cast<CubeCollider*>(other), velocity);

			} else {

				vec3 otherPos = other->position;
				vec3 otherDimen = dynamic_cast<CubeCollider*>(other)->dimensions;

				if (abs(position.x - otherPos.x) < dimensions.x + otherDimen.x) {
					if (abs(position.y - otherPos.y) < dimensions.y + otherDimen.y) {
						if (abs(position.z - otherPos.z) < dimensions.z + otherDimen.z) {
							data.doesIntersect = true;
							data.direction = closestCollidingNormal(other);
							data.amount = velocity;
						}
					}
				}
		}
		break;
		}

	case ColliderTypes::OBBCUBE :
		{
			data = obbCollision(dynamic_cast<CubeCollider*>(other), velocity);
			break;
		}
	}

	return data;
}

vec3 CubeCollider::closestCollidingNormal(Collider* other){
	vec3 direction = position - other->position;
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
	for (int i = 0; i < norms.size(); i++){
		float d = dot(direction, (position + (norms[i] * (dimensions / 2.0f)) - other->position));
		if (d < minCollision){
			closestIndex = i;
			minCollision = d;
		}
	}
	vec3 outNorm = norms[closestIndex];
	if (!isZeroVec3(outNorm)) outNorm = normalize(outNorm);
	return outNorm;
}

IntersectionData CubeCollider::obbCollision(CubeCollider* cube, float velocity){
	IntersectionData data = IntersectionData();
	vec3 offset = cube->position - position;
	vector<vec3> norms = {
		normals[0],
		normals[1],
		normals[2],
		cube->normals[0],
		cube->normals[1],
		cube->normals[2],
		cross(normals[0], cube->normals[0]),
		cross(normals[0], cube->normals[1]),
		cross(normals[0], cube->normals[2]),
		cross(normals[1], cube->normals[0]),
		cross(normals[1], cube->normals[1]),
		cross(normals[1], cube->normals[2]),
		cross(normals[2], cube->normals[0]),
		cross(normals[2], cube->normals[1]),
		cross(normals[2], cube->normals[2]),
		normalize(offset)* -1.0f
	};
	if (isZeroVec3(offset)) norms[norms.size() - 1] = vec3(0.0, 0.0, 0.0);
	data = oBBCollision(*cube, norms);
	if (dot(data.direction, offset) < 0.0f) data.direction *= -1.0f;
	data.amount = velocity;
	data.intersection = position + data.direction * dimensions;

	return data;
}

IntersectionData CubeCollider::sphereToCubeCollision(SphereCollider* sphere, float velocity){
	IntersectionData data = IntersectionData();
	vec3 pt = vec3(0, 0, 0);
	data.doesIntersect = testSphereObb(sphere, *this, pt);
	data.direction = closestCollidingNormal(sphere);
	data.amount = velocity;
	return data;
}

bool CubeCollider::testSphereObb(SphereCollider* sphere, CubeCollider obb, vec3 pt)

{
	pt = obb.closestPtOnOBB(sphere->position);
	vec3 v = pt - sphere->position;
	
	return dot(v, v) <= sphere->radius * sphere->radius;

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

IntersectionData CubeCollider::oBBCollision(CubeCollider other, vector<vec3> norms){
	IntersectionData data = IntersectionData();
	vec3 offset = position - other.position;

	if (magnitude(offset) > magnitude(getCorners()[0] - position) + magnitude(other.getCorners()[0] - other.position)) return data;

		vector<float> depth = {};
		vector<vec3> axis = {};

		int j = 0;

		for (int i = 0; i < norms.size() - 1; i++){
			if(!isZeroVec3(norms[i])){
				pair<bool, float> out = checkProjectedIntersection(getCorners(), other.getCorners(), norms[i], offset);

				if (!out.first) return data;
				axis.push_back( norms[i]);
				depth.push_back( out.second);
				j++;
			}
		}

		int minPos = 0;
		float minVal = std::numeric_limits<float>::max();
		for (int i = 0; i < axis.size(); i++){
			if (abs(depth.at(i)) < minVal){ minVal = depth.at(i); minPos = i; cout << "min pos = " << i << endl; }
		}

		data.doesIntersect = true;
		data.amount = depth[minPos];
		data.direction = normalize(axis[minPos]);
		return data;
}

pair<bool, float> CubeCollider::checkProjectedIntersection(vector<vec3> corners0, vector<vec3> corners1, vec3 axis, vec3 offset){
	float min0 = std::numeric_limits<float>::max();
	float max0 = std::numeric_limits<float>::min();
	float min1 = std::numeric_limits<float>::max();
	float max1 = std::numeric_limits<float>::min();

	if (isZeroVec3(axis))
		return pair<bool, float>(true, std::numeric_limits<float>::max());

		// Calc min and max intervals of each Collider
	for (int i = 0; i < 7; i++) {
		float aDist = dot(corners0[i], axis);
		if (aDist < min0) min0 = aDist; else min0 = min0;
		if (aDist > max0) max0 = aDist; else max0 = max0;
		float bDist = dot(corners1[i], axis);
		if (bDist < min1) min1 = bDist; else min1 = min1;
		if (bDist > max1) max1 = bDist; else max1 = max1;
	}

	float displace = dot(offset, axis);
	min0 += displace;
	max0 += displace;

		// 1D intersection test between 0 and 1
	float longSpan = fmax(max0, max1) - fmin(min0, min1);
	float sumSpan = max0 - min0 + max1 - min1;

	return pair<bool, float>((longSpan <= sumSpan), (sumSpan - longSpan));
}

void CubeCollider::rotate(vec3 axis, float degrees){
	if (colliderType == ColliderTypes::OBBCUBE){
		rotation = Util::rotationMat4(axis, degrees);
		for (int i = 0; i < 2; i++){
			vec4 rotatedNorm = vec4(normals[i], 1.0) * rotation;
			normals[i] = vec3(rotatedNorm.x, rotatedNorm.y, rotatedNorm.z);
		}	
	} else{
		cout << ("Missues of CubeCollider - You cannot rotate a cube that isn't an OBBCUBE") << endl;
	}
}

vector<vec3> CubeCollider::getCorners(){
	vec3 halfDimen = dimensions*2.0f;
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
	}else{
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

float CubeCollider::sqdDistPointAABB(vec3 p, CubeCollider aabb){
			float sq = 0.0;
			sq += sqdValue(p.x, aabb.getMin().x, aabb.getMax().x);
			sq += sqdValue(p.y, aabb.getMin().y, aabb.getMax().y);
			sq += sqdValue(p.z, aabb.getMin().z, aabb.getMax().z);
			sq / 3.0f;

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
