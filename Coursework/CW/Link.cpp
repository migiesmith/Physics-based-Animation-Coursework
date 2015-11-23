/*

Grant Smith (40111906)

TODO

*/

#include "Link.h"

/*
	Create the link

	axis and angle representing the default rotation
	inAngleLimits representing the min and max values the link can rotate (relative to the initial orientation from axis-angle)
*/
Link::Link(vec3 &axis, const float& angle, vector<vec3>* inAngleLimits){
	origin = vec3(0, 0, 0); // Set the origin to zero
	m_quat = angleAxis(angle, axis); // Create a quaternian representing the axis and angle
	m_qWorld = quat(); // Create a world space quaternian for later use
	m_length = 1.0f; // Set the length of the link to 1 (default value)

	// Get the roll, pitch and yaw representing the default orientation and add it to the angleLimits (makes the angle limits relative to the starting orientation)
	vec3 startOri = rollPitchYawFromQuat(m_quat);

	bool validLimits = false;
	if (inAngleLimits != NULL){
		if (inAngleLimits->size() >= 2){
			vec3 minA = inAngleLimits->at(0);
			vec3 maxA = inAngleLimits->at(1);

			angleLimits = { startOri + minA, startOri + maxA };
			validLimits = true;
		}
	}
	if (!validLimits){
		angleLimits = { vec3(-pi<float>(), -pi<float>(), -pi<float>()), vec3(pi<float>(), pi<float>(), pi<float>()) };
	}
	
}

/*
	Create the link

	axis and angle representing the default rotation
	inAngleLimits representing the min and max values the link can rotate (relative to the initial orientation from axis-angle)
	length stating how long the link should be
*/
Link::Link(vec3 &axis, const float& angle, vector<vec3>* inAngleLimits, const float& length) : Link(axis, angle, inAngleLimits){
	m_length = length; // Set the link length to length and pass the rest to the other constructor
}

// Add link l as a child to this link with the identifier name
void Link::addChild(string name, Link* l){
	if (l->parent)
		l->parent->removeChild(l);

	l->parent = this;
	children[name] = l;
}

// remove child called s from this link
void Link::removeChild(string name){
	children.erase(name);
}

// Remove child l from this link
void Link::removeChild(Link* l){
	std::map<string, Link*>::iterator it = children.begin();
	while (it != children.end()) {
		if (it->second == l) {
			children.erase(it++);
			break;
		}
		else {
			++it;
		}
	}
}

// Set this links parent
void Link::setParent(string childName, Link* l){
	if (parent)
		parent->removeChild(this);
	parent = l;
	parent->addChild(childName, this);
}

// Returns the root of this link
Link* Link::getRoot(){
	return parent != NULL ? parent->getRoot() : this;
}

// Reaches towards the target
void Link::privateReach(const Link& endLink, const vec3& target, const float& physicsTimeStep){
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

	if (angle == 0.0f) return;
	
	// Calculate the quaternian of the rotation
	quat qDif = normalize(angleAxis(-angle, axis));


	// Get the current orientation
	quat qCur = normalize(m_qWorld);

	// Get the parent's orientation (if there is a parent)
	quat qPar = quat();
	if (parent)
		qPar = normalize(parent->m_qWorld);

	// Get the local space orientation
	quat qLocal = qCur * conjugate(qPar);

	// Check code - ensure our local-world calculation is correct
	// the dot product should be 1.0!!! - or something went wrong
	//float qd = dot(qLocal, m_quat);

	// Convert world orientation to local space
	quat qNew = normalize( (qCur * qDif) * conjugate(qPar) );
	
	// Slerp and normalize the new rotation quat
	qNew = normalize(slerp(m_quat, qNew, physicsTimeStep));
	
	// Get the roll, pitch and yaw
	vec3 rollPitchYaw = rollPitchYawFromQuat(qNew);

	// Check the roll, pitch and yaw against the angle limits
	if (clamp(rollPitchYaw.x, angleLimits[0].x, angleLimits[1].x) == rollPitchYaw.x && clamp(rollPitchYaw.y, angleLimits[0].y, angleLimits[1].y) == rollPitchYaw.y && clamp(rollPitchYaw.z, angleLimits[0].z, angleLimits[1].z) == rollPitchYaw.z)
		m_quat = qNew; // If the roll, pitch and yaw are within the limits, apply the rotation
	
}

// Reach towards the target and tell the parent to do so as well under certain circumstances
void Link::reach(const vec3& target, const float& physicsTimeStep){
	// Check if the target would cause an error to this link
	if (!Util::equals(target, translationFromMat4(m_base)))
		// Reach for the target
		privateReach(*this, target, physicsTimeStep);

	Link* p = parent;
	int linksUpdated = 0;
	// Loop up through the hierarchy towards the root until the
	// root it reached or until the reach limit for this link is met
	while (p != NULL && (linksUpdated < linkReach || linkReach == -1)){
		// Reach for the target as the parent
		p->privateReach(*this, target, physicsTimeStep);
		// Next parent
		p = p->parent;
		// Keep track of how many we have checked (so we only reach for our limit)
		linksUpdated++;
	}

}

// Update the position of this link and all of its children
void Link::update(){

	// Get the rotation Matrix by converting the quaternion rotation to a matrix
	mat4 R1 = transpose(glm::mat4_cast(m_quat));
	// Get the translation
	mat4 T1 = translationMat4(origin + (parent ? vec3(parent->m_length,0,0) : vec3(0,0,0)));
	// Merge the two matrices
	m_base = T1 * R1;

	// Set the world rotation
	m_qWorld = m_quat;

	// Apply the parent's matrices if there is a parent
	if (parent){
		m_qWorld = m_quat * parent->m_qWorld;
		m_base = parent->m_base * m_base;
	}

	// Normalize the world rotation quaternion
	m_qWorld = normalize(m_qWorld);

	// Update the children of this link
	for (auto& child : children){
		child.second->update();
	}
}

// Render this link and all of its children
void Link::render(const mat4& PV, effect& currentEffect){
	if (toRender){
		vec3 base = translationFromMat4(m_base);
		vec3 end = vec4ToVec3(m_base * vec4(m_length, 0, 0, 1));
		//glUniform4fv(currentEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0.6f, 0.6f, 0.6f, 1)));
		if (_geom)
			Util::renderIKModel(base, end, m_length, m_length, PV, currentEffect, *_geom);
		else
			Util::renderArrow(base, end, m_length, 0.5f, PV, currentEffect); //Util::renderArrow(base, end, m_length, 0.4f, PV, currentEffect);
	}

	for (auto& child : children){
		child.second->render(PV, currentEffect);
	}
}

// Render the debug info for this link and all of its children
void Link::debugRender(const mat4& PV, effect& currentEffect, const vec3& target){
	glUniformMatrix4fv(
		currentEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(PV)); // Pointer to matrix data
	vec3 endVec = vec4ToVec3(m_base * vec4(m_length, 0, 0, 1));
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

	for (auto& child : children){
		child.second->debugRender(PV, currentEffect, target);
	}
}
