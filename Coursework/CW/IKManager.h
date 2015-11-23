/*
Grant Smith (40111906)

Manages all of the ik hierarchies within the project

*/

#pragma once

#include <renderer.h>
#include "Util.h"
#include "IKHierarchy.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class IKManager
{
private:
	
	// Store the managed hierarchies
	map<string, IKHierarchy> _ikHierarchies;
	// Store the texture of the hierarchies
	texture _texture;

public:
	// Create the Manager
	IKManager();

	// Update all hierarchies
	void update();
	// Render all hierarchies
	void render(mat4& VP, effect* shader);
	
	// Override the array operator so that _ikHierarchies can be accessed easily
	IKHierarchy &operator[](const string& id)
	{
		return _ikHierarchies[id];
	}

	// Delete the Manager
	~IKManager();
};

