#pragma once

#include <renderer.h>

using namespace graphics_framework;
using namespace glm;

class Link
{
public:

	quat axisAngle;
	mat4 m_base;

	Link(vec3 &axis, float &angle);
	~Link();
};

