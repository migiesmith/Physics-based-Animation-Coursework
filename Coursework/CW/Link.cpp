#include "Link.h"


Link::Link(vec3 &axis, float &angle){
	origin = vec3(0, 0, 0);
	m_rotation = quat(1, 0, 0, 0);//Util::FromAxisAngle(axis, angle);
}


Link::~Link()
{
}
