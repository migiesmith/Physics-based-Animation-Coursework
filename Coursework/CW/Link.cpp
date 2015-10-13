#include "Link.h"


Link::Link(vec3 &axis, float angle){
	origin = vec3(0, 0, 0);
	m_quat = angleAxis(angle, axis);//Util::FromAxisAngle(axis, angle);
	m_qWorld = quat();
}

Link::Link(vec3 &axis, float angle, float length) : Link(axis, angle){
	m_length = length;
}

void Link::addChild(Link* l){
	children.push_back(l);
}

void Link::removeChild(Link* l){
	children.erase(std::remove(children.begin(), children.end(), l), children.end());
}

void Link::setParent(Link* l){
	if (parent == NULL){
		parent = l;
		parent->addChild(this);
	}else{
		parent->removeChild(this);
		parent = l;
		parent->addChild(this);
	}
}

Link* Link::getRoot(){
	return parent != NULL ? parent->getRoot() : this;
}

void Link::privateReach(Link& endLink, vec3& target){
	vec3 endVec = vec4ToVec3(endLink.m_base * vec4(endLink.m_length, 0, 0, 1));
	//     +
	//    / \
	//   /   \
	//  /     + end
	// + vB    
	//           @ target
	// 
	vec3 vB = translationFromMat4(m_base);


	vec3 v0 = (endVec - vB);
	vec3 v1 = (target - vB);

	v0 = normalize(v0);
	v1 = normalize(v1);

	vec3 axis = cross(v0, v1);

	//axis.y *= -1;

	if (axis.length() < 0.01f) return;


	axis = normalize(axis);

	float angle = dot(v0, v1);

	if (angle < -1) angle = -1;
	if (angle > 1) angle = 1;

	angle = (float)std::acos(angle);

	quat qDif = angleAxis(-angle, axis);
	qDif = normalize(qDif);

	quat qCur = quat();
	//Quaternion qCur2 = Quaternion::FromRotationMatrix( links[i]->m_base );
	qCur = m_qWorld;
	qCur = normalize(qCur);

	quat qPar = quat();
	if (parent)
	{
		//Quaternion qPar2 = Quaternion::FromRotationMatrix( links[i]->m_parent->m_base );
		qPar = parent->m_qWorld;
		qPar = normalize(qPar);
	}


	//Quaternion qLocal = Quaternion::Conjugate(qPar) * qCur;
	quat qLocal = qCur * conjugate(qPar);


	// Check code - ensure our local-world calculation is correct
	// the dot product should be 1.0!!! - or something went wrong
	float qd = dot(qLocal, m_quat);


	// 
	// Magic code - we apply the `correcton' to the world orientation
	// - but most importantly convert the corrected world orientation
	// back to local space
	//
	quat qNew = (qCur * qDif) * conjugate(qPar);
	qNew = normalize(qNew);


	static float val = 0.1f;

	// Use slerp to avoid `snapping' to the target - if 
	// we instead want to `gradually' interpolate to 
	// towards the target
	qNew = slerp(m_quat, qNew, val);
	qNew = normalize(qNew);

	// For 3D ball joint - we use an axis-angle combination
	// could just store a quaternion
	//Vector3 axis2;
	//float angle2 = Quaternion::ToAxisAngle(qNew, axis2);
	//links[i]->m_axis  = axis2;
	//links[i]->m_angle = angle2;
	m_quat = qNew;
}



void Link::reach(vec3& target){
	privateReach(*this, target);

	Link* p = parent;
	int linksUpdated = 0;
	while (p != NULL && (linksUpdated < linkReach || linkReach == -1)){
		p->privateReach(*this, target);
		p = p->parent;
		linksUpdated++;
	}

}


void Link::update(Link& endLink, vec3& target){

	mat4 R1 = transpose(glm::mat4_cast(m_quat));

	mat4 T1 = translationMat4(vec3(m_length, 0, 0) + origin);
	m_base = T1 * R1;

	m_qWorld = m_quat;

	if (parent) m_base = parent->m_base * m_base;

	if (parent) m_qWorld = m_quat * parent->m_qWorld;
	m_qWorld = normalize(m_qWorld);

	for (int i = 0; i < children.size(); i++){
		children[i]->update(endLink, target);
	}

}

void Link::render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target){

	vec3 base = translationFromMat4(m_base);
	vec3 end = vec4ToVec3(m_base * vec4(m_length, 0, 0, 1));
	glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0.6f, 0.6f, 0.6f, 1)));
	Util::renderArrow(base, end, children.size() != 0 ? children[0]->m_length : m_length, 0.4f, PV, currentEffect); //Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect);

	for (int i = 0; i < children.size(); i++){
		children[i]->render(PV, currentEffect, endLink, target);
	}
	
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
	glEnable(GL_DEPTH_TEST);
	vec3 axis = normalize(cross(normalize(curToEnd), normalize(curToTarget)));

	glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0, 0, 1, 1)));
	glBegin(GL_LINES);
	glVertex3f(currVec.x, currVec.y, currVec.z);
	glVertex3f(currVec.x + axis.x, currVec.y + axis.y, currVec.z + axis.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);

}

Link::~Link()
{
}
