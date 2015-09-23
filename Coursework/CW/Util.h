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
	float magnitude(vec3 v);
	bool isZeroVec3(vec3 v);
	vec3 rotate(vec3 v, vec3 axis, float degrees);
	string vec3ToString(vec3 v);
}