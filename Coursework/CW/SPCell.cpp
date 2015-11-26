#include "SPCell.h"

// Check if inCollider intersects and collider in this cell and resolve the collision
void SPCell::intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data){
	// Loop through every collider in this cell
	for (Collider* c : _colliders){
		if (c != &inCollider){
			data.reset();
			// Check for an intersection 
			c->intersects(inCollider, velocity, data);

			//If there was an intersection
			if (data.doesIntersect){
				// Calculate the correcting velocity
				vec3 correctiveVelocity = data.direction * dot(data.direction, (inCollider.velocity - c->velocity)) * 1.45f; // 1.45f is a constant for elasticity
				// If the other collider is static
				if (c->staticPos){
					// only apply the correctiveVelocity to the inCollider
					inCollider.velocity -= (correctiveVelocity);
				}
				else{
					// apply the correctiveVelocity to the inCollider
					inCollider.velocity -= (correctiveVelocity * (inCollider.invMass / c->invMass));
					// apply the correctiveVelocity to the c
					c->velocity -= -(correctiveVelocity* (c->invMass / inCollider.invMass));
				}
				// Add a force relative to the collision direction
				inCollider.addForce(data.direction * dot(data.direction, inCollider.velocity));
				// Add an extra force if the collision was with an object directly below (eg the ground)
				inCollider.addForce(vec3(0, 9.8 * inCollider.mass, 0) * dot(vec3(0, 1, 0), data.direction)*2.0f);
				
			}
			// Increment the counter for tests this update
			testCount++;
		}
	}
	// change the been checked flag to true
	beenChecked = true;
}
