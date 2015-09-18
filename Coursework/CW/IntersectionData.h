/*

Grant Smith (40111906)

TODO

*/

#pragma once

#include <renderer.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;


class IntersectionData
{
public:

	bool doesIntersect = false;
	vec3 direction = vec3(0.0f, 0.0f, 0.0f);
	float amount = 0.0f;

	IntersectionData();
	~IntersectionData();
};

