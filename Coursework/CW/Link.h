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
	quat  m_quat;
	quat  m_qWorld;
	float m_length;
	vec3 origin;

	bool toRender = true;
	bool debugRender = false;
	float priority = 1.0f;
	vector<vec3> angleLimits;

	int linkReach = -1; // Determines how many links this link can affect

	Link* parent;
	map<string, Link*> children;

	Link(vec3& axis, float angle);
	Link(vec3& axis, float angle, float length);

	void setAngleLimits(vec3& min, vec3& max);

	void addChild(string name, Link* l);
	void removeChild(string name);
	void removeChild(Link* l);
	void setParent(string childName, Link* l);
	Link* getRoot();

	void reach(vec3& target, float physicsTimeStep);
	void update();
	void render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target);

	~Link();

private:
	void privateReach(Link& endLink, vec3& target, float physicsTimeStep);
};

