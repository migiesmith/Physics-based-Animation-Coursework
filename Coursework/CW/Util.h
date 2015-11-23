/*

Grant Smith (40111906)

A utility class to keep commonly used functions

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

	// Initialise the util class
	void init();


	// Returns the magnitude vec3 v
	float magnitude(const vec3& v);
	// Check if vec3 v is a zero vector (0,0,0)
	bool isZeroVec3(const vec3& v);
	// Checks if v0 and v1 are equal
	bool equals(const vec3& v0, const vec3& v1);

	// Returns the result of this vector rotated by degrees around axis
	vec3 rotate(const vec3& v, const vec3& axis, float degrees);
	// Convert vec3 v to a string
	string vec3ToString(const vec3& v);
	// Creates a rotation matrix from an axis and an angle
	mat4 rotationMat4(vec3& axis, float degrees);
	// Returns a vec4 representing vec3 v
	vec4 vec3ToVec4(const vec3& v);
	// Returns a vec3 representing vec4 v
	vec3 vec4ToVec3(const vec4& v);

	// Render an arrow
	void renderArrow(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& VP, effect& currentEffect);
	// Render another model like the arrow is rendered (used for ik rendering)
	void renderIKModel(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect, const geometry& geom);
	// Set up model rendering (for renderArrow and renderIKModel)
	void setUpModelRendering(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect);

	// Return the translation part of mat4 m
	vec3 translationFromMat4(const mat4& m);
	// Create a translation matrix from v
	mat4 translationMat4(const vec3& v);

	// Returns a vec3 representing the rotation of quat q as vec3(roll, pitch, yaw)
	vec3 rollPitchYawFromQuat(const quat& q);
	
}