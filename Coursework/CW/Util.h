/*

Grant Smith (40111906)

TODO

*/

#pragma once

#include <renderer.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

namespace Util
{
	// Loads in a model and returns it
	geometry loadModel(string modelName);

	void init();
	float magnitude(vec3 v);
	bool isZeroVec3(vec3 v);
	vec3 rotate(vec3 v, vec3 axis, float degrees);
	string vec3ToString(vec3 v);
	mat4 rotationMat4(vec3& axis, float degrees);
	vec4 vec3ToVec4(vec3 v);
	vec3 vec4ToVec3(vec4 v);
	void renderArrow(vec3 start, vec3 end, float length, float radius, mat4& VP, effect currentEffect);
	mat4 mult(const mat4& a, const mat4& b);
	vec3 translationFromMat4(mat4& m);
	mat4 translationMat4(vec3& v);
	
	/* TODO
	vec3 GetMousePosition2Dto3D();
	*/

}