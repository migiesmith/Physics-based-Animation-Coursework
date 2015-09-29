#include "Link.h"


Link::Link(vec3 &axis, float &angle){
	//axisAngle = quat(angle, axis);
	m_axis = axis;
	m_angle = angle;
}


Link::~Link()
{
}
