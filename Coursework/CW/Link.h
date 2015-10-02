#pragma once

#include <renderer.h>

using namespace graphics_framework;
using namespace glm;

class Link
{
public:

	vec3 m_axis;
	float m_angle;
	mat4 m_base;
	vec3 origin;

	Link(vec3 &axis, float &angle);
	~Link();
};

