/*

Grant Smith (40111906)

TODO

*/

#pragma once

#include <renderer.h>
#include "Util.h"

using namespace graphics_framework;
using namespace glm;
using namespace Util;

class Link
{
private:
	
	geometry* _geom;
	void privateReach(const Link& endLink, const vec3& target, const float& physicsTimeStep);

public:

	mat4 m_base;
	quat  m_quat;
	quat  m_qWorld;
	float m_length;
	vec3 origin;

	bool ignoreCollision;
	bool toRender = true;
	bool debugRender = false;
	vector<vec3> angleLimits;

	int linkReach = -1; // Determines how many links this link can affect

	Link* parent;
	map<string, Link*> children;

	Link(vec3& axis, const float& angle, vector<vec3>* angleLimits);
	Link(vec3& axis, const float& angle, vector<vec3>* angleLimits, const float& length);
	
	void addChild(string name, Link* l);
	void removeChild(string name);
	void removeChild(Link* l);
	void setParent(string childName, Link* l);
	Link* getRoot();

	void setGeometry(geometry* geom){ _geom = geom; }

	void reach(const vec3& target, const float& physicsTimeStep);
	void update();
	void render(const mat4& PV, effect& currentEffect);

	~Link();

};

