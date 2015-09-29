#include "Link.h"


Link::Link(vec3 &axis, float &angle){
	axisAngle = quat(angle, axis);
}


Link::~Link()
{
}
