#include "Link.h"


Link::Link(vec3 &axis, float angle){
	origin = vec3(0, 0, 0);
	m_quat = angleAxis(angle, axis);//Util::FromAxisAngle(axis, angle);
	m_qWorld = quat();
	m_length = 1.0f;
	angleLimits = { vec3(-pi<float>(), -pi<float>(), -pi<float>()), vec3(pi<float>(), pi<float>(), pi<float>()) };
}

Link::Link(vec3 &axis, float angle, float length) : Link(axis, angle){
	m_length = length;
}

void Link::setAngleLimits(vec3& min, vec3& max){
	angleLimits[0] = min;
	angleLimits[1] = max;
}

void Link::addChild(string s, Link* l){
	if (l->parent)
		l->parent->removeChild(l);

	l->parent = this;
	children[s] = l;
}

void Link::removeChild(string s){
	children[s]->~Link();
	//children.erase(std::remove(children.begin(), children.end(), l), children.end());
}

void Link::removeChild(Link* l){
	
}

void Link::setParent(string childName, Link* l){
	if (parent)
		parent->removeChild(this);
	parent = l;
	parent->addChild(childName, this);
}

Link* Link::getRoot(){
	return parent != NULL ? parent->getRoot() : this;
}

void Link::privateReach(Link& endLink, vec3& target, float physicsTimeStep){
	// The position of the end of the end link
	vec3 endVec = vec4ToVec3(endLink.m_base * vec4(endLink.m_length, 0, 0, 1));
	//     +
	//    / \
	//   /   \
	//  /     + end
	// + curr    
	//           @ target
	//

	// Get the vectors required for rotating the link
	vec3 currPos = translationFromMat4(m_base);
	vec3 currToEnd = normalize(endVec - currPos);
	vec3 currToTarget = normalize(target - currPos);
	
	// Calculate the axis of rotation
	vec3 axis = cross(currToEnd, currToTarget);
	// Check if the axis of rotation is valid
	if (axis.length() < 0.01f) return;
	// Normalize the axis
	axis = normalize(axis);

	// Calculate the rotation angle
	float angle = acos( fmin(fmax(dot(currToEnd, currToTarget), -1.0f), 1.0f) );

	angle *= priority;
	if (angle == 0.0f) return;
	
	// Calculate the quaternian of the rotation
	quat qDif = normalize(angleAxis(-angle, axis));


	// Get the current orientation
	quat qCur = normalize(m_qWorld);

	// Get the parent's orientation (if there is a parent)
	quat qPar = quat();
	if (parent)
	{
		qPar = normalize(parent->m_qWorld);
	}

	// Get the local space orientation
	quat qLocal = qCur * conjugate(qPar);

	// Check code - ensure our local-world calculation is correct
	// the dot product should be 1.0!!! - or something went wrong
	//float qd = dot(qLocal, m_quat);

	// Convert world orientation to local space
	quat qNew = normalize( (qCur * qDif) * conjugate(qPar) );

	/*
	vec3 newAx;
	newAx.x = fmin(fmax(qNew.x, angleLimits[0].x), angleLimits[1].x);
	newAx.y = fmin(fmax(qNew.y, angleLimits[0].y), angleLimits[1].y);
	newAx.z = fmin(fmax(qNew.z, angleLimits[0].z), angleLimits[1].z);
	qNew = qNew;

	vec3 newAx;
	float newAng = ToAxisAngle(qNew, newAx);
	newAx *= newAng;
	newAx.x = clamp(newAx.x, angleLimits[0].x, angleLimits[1].x);
	newAx.y = clamp(newAx.y, angleLimits[0].y, angleLimits[1].y);
	newAx.z = clamp(newAx.z, angleLimits[0].z, angleLimits[1].z);
	qNew = normalize(angleAxis(newAng, newAx));
	cout << vec3ToString(newAx) << endl;
	*/


	// Use slerp to avoid `snapping' to the target - if 
	// we instead want to `gradually' interpolate to 
	// towards the target

	qNew = normalize(slerp(m_quat, qNew, physicsTimeStep));

	float roll = atan2(2 * qNew.y*qNew.w - 2 * qNew.x*qNew.z, 1 - 2 * qNew.y*qNew.y - 2 * qNew.z*qNew.z);
	float pitch = atan2(2 * qNew.x*qNew.w - 2 * qNew.y*qNew.z, 1 - 2 * qNew.x*qNew.x - 2 * qNew.z*qNew.z);
	float yaw = asin(2 * qNew.x*qNew.y + 2 * qNew.z*qNew.w);

	if (clamp(roll, angleLimits[0].x, angleLimits[1].x) == roll && clamp(pitch, angleLimits[0].y, angleLimits[1].y) == pitch && clamp(yaw, angleLimits[0].z, angleLimits[1].z) == yaw)
		m_quat = qNew;
	
}



void Link::reach(vec3& target, float physicsTimeStep){
	privateReach(*this, target, physicsTimeStep);

	Link* p = parent;
	int linksUpdated = 0;
	while (p != NULL && (linksUpdated < linkReach || linkReach == -1)){
		p->privateReach(*this, target, physicsTimeStep);
		p = p->parent;
		linksUpdated++;
	}

}


void Link::update(){

	mat4 R1 = transpose(glm::mat4_cast(m_quat));
	mat4 T1 = translationMat4(origin + (parent ? vec3(parent->m_length,0,0) : vec3(0,0,0)));
	m_base = T1 * R1;
	
	m_qWorld = m_quat;

	if (parent) m_base = parent->m_base * m_base;

	if (parent) m_qWorld = m_quat * parent->m_qWorld;
	m_qWorld = normalize(m_qWorld);

	for (auto& child : children){
		child.second->update();
	}
}

void Link::render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target){
	if (toRender){
		vec3 base = translationFromMat4(m_base);
		vec3 end = vec4ToVec3(m_base * vec4(m_length, 0, 0, 1));
		glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0.6f, 0.6f, 0.6f, 1)));
		Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect); //Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect);

		if (debugRender){
			glUniformMatrix4fv(
				currentEffect.get_uniform_location("MVP"), // Location of uniform
				1, // Number of values - 1 mat4
				GL_FALSE, // Transpose the matrix?
				value_ptr(PV)); // Pointer to matrix data
			vec3 endVec = vec4ToVec3(endLink.m_base * vec4(endLink.m_length, 0, 0, 1));
			vec3 currVec = Util::translationFromMat4(m_base);
			vec3 curToEnd = normalize(endVec - currVec);
			vec3 curToTarget = normalize(target - currVec);

			glDisable(GL_DEPTH_TEST);
			glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(1, 0, 0, 1)));
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			glVertex3f(currVec.x, currVec.y, currVec.z);
			glVertex3f(currVec.x + curToEnd.x, currVec.y + curToEnd.y, currVec.z + curToEnd.z);
			glEnd();
			glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0, 1, 0, 1)));
			glBegin(GL_LINES);
			glVertex3f(currVec.x, currVec.y, currVec.z);
			glVertex3f(currVec.x + curToTarget.x, currVec.y + curToTarget.y, currVec.z + curToTarget.z);
			glEnd();

			if (equals(curToEnd, curToTarget)){
				vec3 axis = normalize(cross(normalize(curToEnd), normalize(curToTarget)));
				glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0, 0, 1, 1)));
				glBegin(GL_LINES);
				glVertex3f(currVec.x, currVec.y, currVec.z);
				glVertex3f(currVec.x + axis.x, currVec.y + axis.y, currVec.z + axis.z);
				glEnd();
			}
			glEnable(GL_DEPTH_TEST);
		}
	}

	for (auto& child : children){
		child.second->render(PV, currentEffect, endLink, target);
	}
}

Link::~Link()
{
}
