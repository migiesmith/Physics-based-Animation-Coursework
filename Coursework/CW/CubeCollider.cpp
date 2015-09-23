/*

Grant Smith (40111906)

TODO

*/

#include "CubeCollider.h"



IntersectionData CubeCollider::intersects(Collider* other){
	IntersectionData data = IntersectionData();

	switch(other->colliderType){
	case ColliderTypes::SPHERE : {
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(other);
				if (colliderType == ColliderTypes::OBBCUBE) {

					vector<vec3> corners = getCorners();

					vec3 max = corners[0];
					vec3 min = corners[6];
					float r = sphere->radius;
					vec3 center = sphere->position;

					float r2 = r * r;
					float delta = 0.0;
					for (int i = 0; i < 2; i++){
						if (center[i] < min[i]) delta += (center[i] - min[i]) * (center[i] - min[i]);
						else if (center[i] > max[i]) delta += (center[i] - max[i]) * (center[i] - max[i]);
					}
					data.doesIntersect = delta <= r2;
					data.amount = r2 - delta;
					data.direction = (center - position);
					if (!isZeroVec3(data.direction)) data.direction = normalize(data.direction);

				}
				else { //AABB Cube

					data.doesIntersect = (sqdDistPointAABB(sphere->position, *this) <= (sphere->radius * 2.0f));

					vec3 direction = position - sphere->position;
					if (!isZeroVec3(direction))
						direction = normalize(direction);

					data.amount = 10000.0f;
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
						float d = dot(direction, (position + (norms[i] * (dimensions / 2.0f)) - sphere->position));
						if (d < data.amount){
							closestIndex = i;
							data.amount = d;
						}
					}
					direction = norms[closestIndex];
					if (data.amount < 0)data.amount += sphere->radius; else data.amount -= sphere->radius;
					
					cout << Util::vec3ToString(direction) << endl;
					


					//TODO FIX this bit
					if (!isZeroVec3(direction)){
						direction = normalize(direction);
						cout << data.amount << "|||" << endl;
					}

					data.direction = direction;
				}
				break;
		}

	case ColliderTypes::CUBE :
		{

		if (colliderType == ColliderTypes::OBBCUBE){
				CubeCollider* otherCollider = dynamic_cast<CubeCollider*>(other);
				vec3 offset = other->position - position;
				vector<vec3> norms = {
					normals[0],
					normals[1],
					normals[2],
					otherCollider->normals[0],
					otherCollider->normals[1],
					otherCollider->normals[2],
					cross(normals[0], otherCollider->normals[0]),
					cross(normals[0], otherCollider->normals[1]),
					cross(normals[0], otherCollider->normals[2]),
					cross(normals[1], otherCollider->normals[0]),
					cross(normals[1], otherCollider->normals[1]),
					cross(normals[1], otherCollider->normals[2]),
					cross(normals[2], otherCollider->normals[0]),
					cross(normals[2], otherCollider->normals[1]),
					cross(normals[2], otherCollider->normals[2]),
					normalize(offset)* -1.0f
				};
				if (isZeroVec3(offset)) norms[norms.size() - 1] = vec3(0.0, 0.0, 0.0);
					data = oBBCollision(*otherCollider, norms);

			}
		else {
				vec3 otherPos = other->position;
				vec3 otherDimen = dynamic_cast<CubeCollider*>(other)->dimensions;

				if (abs(position.x - otherPos.x) < dimensions.x + otherDimen.x) {
					if (abs(position.y - otherPos.y) < dimensions.y + otherDimen.y) {
						if (abs(position.z - otherPos.z) < dimensions.z + otherDimen.z) {
							data.doesIntersect = true;

							vec3 direction = position - otherPos;
							if (!isZeroVec3(direction))
								direction = normalize(direction);

							if (abs(direction.x) >= 0.5) {
								direction = vec3(round(direction.x), 0.0, 0.0);
							}
							if (abs(direction.y) >= 0.5) {
								direction = vec3(0.0, round(direction.y), 0.0);
							}
							if (abs(direction.z) >= 0.5) {
								direction = vec3(0.0, 0.0, round(direction.z));
							}
							
							data.direction = direction;
						}
					}
				}
		}
		break;
		}

	case ColliderTypes::OBBCUBE :
		{
			CubeCollider* otherCollider = dynamic_cast<CubeCollider*>(other);

			vec3 offset = other->position - position;
			vector<vec3> norms = {
				normals[0],
				normals[1],
				normals[2],
				otherCollider->normals[0],
				otherCollider->normals[1],
				otherCollider->normals[2],
				cross(normals[0], otherCollider->normals[0]),
				cross(normals[0], otherCollider->normals[1]),
				cross(normals[0], otherCollider->normals[2]),
				cross(normals[1], otherCollider->normals[0]),
				cross(normals[1], otherCollider->normals[1]),
				cross(normals[1], otherCollider->normals[2]),
				cross(normals[2], otherCollider->normals[0]),
				cross(normals[2], otherCollider->normals[1]),
				cross(normals[2], otherCollider->normals[2]),
				normalize(offset)* -1.0f
			};
			if (isZeroVec3(offset)) norms[norms.size() - 1] = vec3(0.0, 0.0, 0.0);
				data = oBBCollision(*otherCollider, norms);

				break;
		}
	}

	return data;
}

IntersectionData CubeCollider::oBBCollision(CubeCollider other, vector<vec3> norms){
	IntersectionData data = IntersectionData();
	vec3 offset = other.position - position;

	if (magnitude(offset) > magnitude(getCorners()[0] - position) + magnitude(other.getCorners()[0] - other.position)) return data;



		vector<float> depth = {};
		vector<vec3> axis = {};

		int j = 0;

		for (int i = 0; i < norms.size() - 1; i++){
		//if(!norms[i].isZero()){
			pair<bool, float> out = checkProjectedIntersection(getCorners(), other.getCorners(), norms[i], offset);

			if (!out.first) return data;
			axis.assign(j, norms[i]);
			depth[j] = out.second;
			//}
			j++;
		}


		int minPos = 0;
		float minVal = std::numeric_limits<float>::max();
		for (int i = 0; i < norms.size() - 1; i++){
			if (&axis[i])if (abs(depth[i]) < minVal){ minVal = depth[i]; minPos = i; }
		}

		data.doesIntersect = true;
		data.amount = depth[minPos];
		data.direction = axis[minPos];
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
			for (int i = 0; i < 2; i++)
				normals[i] = Util::rotate(normals[i], axis, degrees);
		}
		else{
			cout << ("Missues of CubeCollider - You cannot rotate a cube that isn't an OBBCUBE") << endl;
		}
	}

vector<vec3> CubeCollider::getCorners(){
	vec3 halfDimen = dimensions / 2.0f;

	//TODO Add in the rotation code here? if(colliderType == ColliderTypes.OBBCUBE)

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
