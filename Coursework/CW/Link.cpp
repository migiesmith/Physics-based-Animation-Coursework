#include "Link.h"


Link::Link(vec3 &axis, float angle){
	origin = vec3(0, 0, 0);
	m_rotation = quat(1, 0, 0, 0);//Util::FromAxisAngle(axis, angle);
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

	if (pow(magnitude(target - endVec), 2.0f) < 0.1f) return;

	// These lines are perpendicular, no axis of rotation can be found
	if (abs(dot(curToEnd, curToTarget)) == 1.0f) return;

	vec3 axis = normalize(cross(curToEnd, curToTarget));

	float angle = acos(dot(curToEnd, curToTarget));
	float ax = dot(curToEnd, curToTarget);// / (magnitude(curToEnd) * magnitude(curToTarget));
	ax = glm::min(1.0f, glm::max(ax, -1.0f));
	ax = (float)acos(ax);

	ax = glm::min(.5f, glm::max(ax, -0.5f));

	if (abs(ax) < 0.01f) return;
	
	quat qCur = m_rotation;

	quat qDif = angleAxis(-ax, axis);

	float s0 = qCur.w;
	float s1 = qDif.w;
	vec3 v0 = vec3(qCur.x, qCur.y, qCur.z);
	vec3 v1 = vec3(qDif.x, qDif.y, qDif.z);
	quat qNew = normalize(qCur * qDif);//normalize(qCur * qDif);
	
	if (this == &endLink)cout << " | " << qNew.w << ", " << qNew.x << ", " << qNew.y << ", " << qNew.z << endl;
	qNew = slerp(qCur, qNew, 0.06f);
	m_rotation = qNew;



	/*
	float m = sqrt(2.f + 2.f * dot(curToEnd, curToTarget));
	vec3 w = (1.f / m) * cross(curToEnd, curToTarget);
	m_rotation = quat(0.5f * m, w.x, w.y, w.z);
	*/
}



void Link::update(Link& endLink, vec3& target){

	mat4 rot = quatToMat4(m_rotation);//Util::rotationMat4(links[i]->m_axis, links[i]->m_angle);
	mat4 trans = Util::translationMat4((vec3(m_length, 0, 0) + origin));

	m_base = mult(rot, trans);
	if (parent != NULL) m_base = mult(m_base, parent->m_base);

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
