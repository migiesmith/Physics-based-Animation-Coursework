#include "Link.h"


Link::Link(vec3 &axis, float angle, float length){
	m_length = length;
	origin = vec3(0, 0, 0);
	//m_rotation = quat(1, 0, 0, 0);//Util::FromAxisAngle(axis, angle);
	m_local = mat4();
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

void Link::reach(Link& endLink, vec3& target){
	vec3 endVec = vec4ToVec3(endLink.m_base * vec4(endLink.m_length, 0, 0, 1));
	//     +
	//    / \
	//   /   \
	//  /     + end
	// + vB    
	//           @ target
	// 
	vec3 currVec = Util::translationFromMat4(m_base);
	
	vec3 curToEnd = normalize(endVec - currVec);
	vec3 curToTarget = normalize(target - currVec);


	// These lines are perpendicular, no axis of rotation can be found

	vec3 axis = cross(curToEnd, curToTarget);
	if (magnitude(axis) < 0.1f) return;
	axis = normalize(axis);
	//axis.y = -axis.y;

	float ax = dot(curToEnd, curToTarget);// / (magnitude(curToEnd) * magnitude(curToTarget));
	ax = glm::min(1.0f, glm::max(ax, -1.0f));
	ax = (float)acos(ax);

	//ax = glm::min(.5f, glm::max(ax, -0.5f));

	
	//quat qCur = m_rotation;

	//quat qDif = angleAxis(-ax, axis);

	//quat qNew = normalize(qCur * qDif);//normalize(qCur * qDif);
	

	//if (this == &endLink)cout << " | " << qNew.w << ", " << qNew.x << ", " << qNew.y << ", " << qNew.z << endl;
	//qNew = slerp(qCur, qNew, 0.1f);
	//m_rotation = qNew;

	mat4 mDif;
	mDif = glm::rotate(mDif, ax*0.1f, axis);

	m_local = mDif * m_local;

	/*
	float m = sqrt(2.f + 2.f * dot(curToEnd, curToTarget));
	vec3 w = (1.f / m) * cross(curToEnd, curToTarget);
	m_rotation = quat(0.5f * m, w.x, w.y, w.z);
	*/
}



void Link::update(Link& endLink, vec3& target){

	if (parent != NULL){
		m_base = parent->m_base * translationMat4(vec3(m_length, 0, 0) + origin) * m_local;
	}

	cout << vec3ToString(translationFromMat4(m_base)) << endl;

	for (int i = 0; i < children.size(); i++){
		children[i]->update(endLink, target);
	}
	reach(endLink, target);

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

	vec3 axis = normalize(cross(curToEnd, curToTarget));

	glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0, 0, 1, 1)));
	glBegin(GL_LINES);
	glVertex3f(currVec.x, currVec.y, currVec.z);
	glVertex3f(currVec.x + axis.x, currVec.y + axis.y, currVec.z + axis.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);

	glBegin(GL_POINTS);
	glVertex3f(end.x, end.y, end.z);
	glEnd();
}

Link::~Link()
{
}
