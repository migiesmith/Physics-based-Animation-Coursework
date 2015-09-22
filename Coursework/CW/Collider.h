/*

Grant Smith (40111906)

TODO

*/

#pragma once

#include <renderer.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;


enum class ColliderTypes :short{
	CUBE,
	SPHERE,
	OBBCUBE
};

class Collider
{
private:
	bool _solid = true;

public:

	vec3 position;
	ColliderTypes colliderType;

	virtual bool Collider::isSolid();
	Collider(vec3 position, ColliderTypes colliderType);
	void setPosition(vec3 position);
	void translate(vec3 translation);
	ColliderTypes getType(){ return colliderType; }
	~Collider();
};
