/*

	Grant Smith (40111906)

	Stores the transform and mesh of the object for the scene.
	Stores the collider for the object so that it can use collisions

*/

#pragma once

#include <texture.h>
#include <mesh.h>
#include <renderer.h>
#include "Collider.h"

#include <list>
#include <iostream>

using namespace std;
using namespace graphics_framework;
using namespace glm;

class SceneObject : public mesh
{
private:
	texture* _texture; // This objects texture
	Collider* _collider = NULL; // This objects texture
	texture* _normal = NULL; // The normal map of this object
	
public:

	vec3 velocity, force; // Velocity and Force

	// Constructor for the scene object
	SceneObject(){
		velocity = vec3(0, 0, 0);
		force = vec3(0, 0, 0);
	}

	// Creates a mesh object with the provided geometry
	SceneObject(geometry &geom) : mesh(geom){ }
	SceneObject(mesh &m) : mesh((geometry)m.get_geometry()){ }
	// Creates a mesh object with the provided geometry and material
	SceneObject(geometry &geom, material &mat) : mesh(geom, mat){ }
	// Default copy constructor and assignment operator
	SceneObject(const SceneObject &other) : mesh((geometry)other.get_geometry()) {
		_texture = other._texture;
		_normal = other._normal;
	}
	
	// Set the scene object's texture
	void set_texture(texture &value) { _texture = &value; }
	// Returns the scene object's texture
	texture get_texture() { return *_texture; }
	// Set the scene object's normal texture
	void set_normal_texture(texture &value) { _normal = &value; }

	// Sets all of the material values 
	void set_material(glm::vec4 &emissive, glm::vec4 &diffuse, glm::vec4 &specular, float shininess){
		get_material().set_emissive(emissive);
		get_material().set_diffuse(diffuse);
		get_material().set_specular(specular);
		get_material().set_shininess(shininess);
	}

	// Update the scene object
	void update(float delta_time);

	void setCollider(Collider* c){ _collider = c; }
	Collider* getCollider(){ return _collider; }

	// Checks for an intersection between this object's collider and collider c
	void intersects(Collider& c, const vec3& velocity, IntersectionData& data);

	// Render the mesh of the object
	void render(const mat4& VP, const effect& shader);
	
	// Delete the sceneobject
	~SceneObject(){}

};
