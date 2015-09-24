/*

Grant Smith (40111906)

TODO

*/

#include <math.h>
#include <renderer.h>
#include "Util.h"

using namespace glm;

using namespace Util;

	//Returns the magnitude vec3 v
	float Util::magnitude(vec3 v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}

	//Checks if vec3 v is a zero vector
	bool Util::isZeroVec3(vec3 v){
		return v.x == 0.0 && v.y == 0.0 && v.z == 0.0;
	}
	

	// Returns the result of this vector rotated by degrees around axis
	vec3 Util::rotate(vec3 v, vec3 axis, float degrees){
		// Convert the input degrees to radians
		float angleInRad = radians(degrees);
		// Calculate the rotation around each axis
		float a = axis.x * angleInRad;
		float b = axis.y * angleInRad;
		float c = axis.z * angleInRad;

		// Create the rotation matrix using the angles a, b, and c
		mat4 rotationMat = mat4(
			cos(b)*cos(c), cos(c)*sin(a)*sin(b) - cos(a)*sin(c), cos(a)*cos(c)*sin(b) + sin(a)*sin(c), 0.0,
			cos(b)*sin(c), cos(a)*cos(c) + sin(a)*sin(b)*sin(c), -cos(c)*sin(a) + cos(a)*sin(b)*sin(c), 0.0,
			-sin(b), cos(b)*sin(a), cos(a)*cos(b), 0.0,
			0.0, 0.0, 0.0, 1.0
			);

		//Rotates the vector using a vec4
		vec4 rotatedVec3 = vec4(v.x, v.y, v.z, 1.0) * rotationMat;
		//Converts the vec4 back to a vec3 and returns it
		return vec3(rotatedVec3.x, rotatedVec3.y, rotatedVec3.z);
	}

	mat4 Util::rotationMat4(vec3 axis, float degrees){
		// Convert the input degrees to radians
		float angleInRad = radians(degrees);
		// Calculate the rotation around each axis
		float a = axis.x * angleInRad;
		float b = axis.y * angleInRad;
		float c = axis.z * angleInRad;

		// Create the rotation matrix using the angles a, b, and c
		mat4 rotationMat = mat4(
			cos(b)*cos(c), cos(c)*sin(a)*sin(b) - cos(a)*sin(c), cos(a)*cos(c)*sin(b) + sin(a)*sin(c), 0.0,
			cos(b)*sin(c), cos(a)*cos(c) + sin(a)*sin(b)*sin(c), -cos(c)*sin(a) + cos(a)*sin(b)*sin(c), 0.0,
			-sin(b), cos(b)*sin(a), cos(a)*cos(b), 0.0,
			0.0, 0.0, 0.0, 1.0
			);

		//Returns the rotation matrix
		return rotationMat;
	}

	vec4 Util::vec3ToVec4(vec3 v){
		return vec4(v.x, v.y, v.z, 1.0);
	}
	vec3 Util::vec4ToVec3(vec4 v){
		return vec3(v.x / v.w, v.y / v.w, v.z / v.w);
	}

	string Util::vec3ToString(vec3 v){
		return to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z);
	}