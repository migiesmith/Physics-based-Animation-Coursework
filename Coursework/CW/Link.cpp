#include "Link.h"


Link::Link(vec3 &axis, float angle){
	origin = vec3(0, 0, 0);
	m_quat = angleAxis(angle, axis);//Util::FromAxisAngle(axis, angle);
	m_qWorld = quat();
	m_length = 1.0f;
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
	
	// Calculate the quaternian of the rotation
	quat qDif = normalize( angleAxis(-angle, axis) );

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

	// Use slerp to avoid `snapping' to the target - if 
	// we instead want to `gradually' interpolate to 
	// towards the target
	m_quat = normalize(slerp(m_quat, qNew, physicsTimeStep));
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


	for (int i = 0; i < children.size(); i++){
		children[i]->update();
	}

}

void Link::render(mat4& PV, effect& currentEffect, Link& endLink, vec3& target){

	vec3 base = translationFromMat4(m_base);
	vec3 end = vec4ToVec3(m_base * vec4(m_length, 0, 0, 1));
	glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0.6f, 0.6f, 0.6f, 1)));
	Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect); //Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect);

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
