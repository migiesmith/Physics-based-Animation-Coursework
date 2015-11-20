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
	
	map<string, IKHierarchy> _ikHierarchies;
	texture _texture;

public:
	IKManager();

	void update();
	void render(mat4& VP, effect* shader);
	
	IKHierarchy &operator[](const string& id)
	{
		return _ikHierarchies[id];
	}

	~IKManager();
};

