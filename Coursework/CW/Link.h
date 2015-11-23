/*

Grant Smith (40111906)

The individual link of the ik hierarchy

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
	
	// A pointer to the model for this link
	geometry* _geom;
	// Reaches towards the target
	void privateReach(const Link& endLink, const vec3& target, const float& physicsTimeStep);

public:

	mat4 m_base; // the matrix representation of this link
	quat  m_quat; // the local rotation of this link
	quat  m_qWorld; // the world rotation of this link
	float m_length; // the length of this link
	vec3 origin; // The offset from the origin

	// whether or not to render this link
	bool toRender = true;
	vector<vec3> angleLimits;

	int linkReach = -1; // Determines how many links this link can affect

	// The parent of this link
	Link* parent;
	// The children of this link
	map<string, Link*> children;

	// Create the link
	Link(vec3& axis, const float& angle, vector<vec3>* angleLimits);
	Link(vec3& axis, const float& angle, vector<vec3>* angleLimits, const float& length);

	// Add link l as a child to this link with the identifier s
	void addChild(string name, Link* l);
	// remove child called s from this link
	void removeChild(string name);
	// Remove child l from this link
	void removeChild(Link* l);
	// Set this links parent
	void setParent(string childName, Link* l);
	// Returns the root of this link
	Link* getRoot();

	void setGeometry(geometry* geom){ _geom = geom; }

	// Reach towards the target and tell the parent to do so as well under certain circumstances
	void reach(const vec3& target, const float& physicsTimeStep);
	// Update the position of this link and all of its children
	void update();
	// Render this link and all of its children
	void render(const mat4& PV, effect& currentEffect);
	// Render the debug info for this link and all of its children
	void Link::debugRender(const mat4& PV, effect& currentEffect, const vec3& target);

	// Delete the link
	~Link(){}

};

