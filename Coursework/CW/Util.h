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
	// Loads in a texture and returns it
	texture loadTexture(string textureName, bool mipmaps, bool antisoptrics);
	// Loads in a texture and returns it
	texture loadTexture(string textureName);

	void init();
	float magnitude(const vec3& v);
	float sqdMagnitude(const vec3& v);
	bool isZeroVec3(vec3 v);
	bool equals(vec3 v0, vec3 v1);

	float ToAxisAngle(const quat& q, vec3& v);
	vec3 rotate(vec3 v, vec3 axis, float degrees);
	string vec3ToString(vec3 v);
	mat4 rotationMat4(vec3& axis, float degrees);
	vec4 vec3ToVec4(vec3 v);
	vec3 vec4ToVec3(vec4 v);

	void renderArrow(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& VP, effect& currentEffect);
	void renderPlane(const vec3& pos, const vec3 scale, const mat4& VP, effect& currentEffect);

	mat4 mult(const mat4& a, const mat4& b);
	quat mult(quat q0, quat q1);
	vec3 translationFromMat4(mat4& m);
	mat4 translationMat4(vec3& v);
	mat4 quatToMat4(quat& q);
	quat FromAxisAngle(const vec3& v, float angle);

	vec3 rollPitchYawFromQuat(const quat& q);

	pair<vec3, vec3> rayFromScreen(vec3& camPos, vec2& clickPos);

	/* TODO
	vec3 GetMousePosition2Dto3D();
	*/

}