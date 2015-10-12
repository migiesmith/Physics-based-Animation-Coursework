#pragma once

#include <renderer.h>
#include "Util.h"

using namespace graphics_framework;
using namespace glm;
using namespace Util;

class Link
{
public:

	mat4 m_base;
	//quat m_rotation;
	float m_length;
	vec3 origin;

	mat4 m_local;

	Link* parent;
	vector<Link*> children;

	Link(vec3& axis, float angle, float length);

	void addChild(Link* l);
	void removeChild(Link* l);
	void setParent(Link* l);
	Link* getRoot();

	void reach(Link& endLink, vec3& target);
	void update(Link& endLink, vec3& target);
	void render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target);

	~Link();
};

