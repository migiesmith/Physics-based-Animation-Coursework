#pragma once

#include <renderer.h>
#include "Util.h"

using namespace graphics_framework;
using namespace glm;

class Link
{
public:

	mat4 m_base;
	quat m_rotation;
	vec3 origin;

	Link(vec3 &axis, float &angle);
	~Link();
};

