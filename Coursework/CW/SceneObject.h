/*

	Grant Smith (40111906)

	Stores the transform and mesh of the object for the scene.
	Also stores the parent and child of itself so that sceneobjects can be attatched to eachother

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

	SceneObject();

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

	SceneObject& operator=(const SceneObject &rhs) = default;
	// Destroys the mesh object
	~SceneObject();

	void set_texture(texture &value) { _texture = &value; }
	texture get_texture() { return *_texture; }
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

	void setCollider(Collider& c){ _collider = &c; }

	void intersects(Collider& c, const vec3& velocity, IntersectionData& data);

	// Render the mesh of the object
	void render(const mat4& VP, const effect& shader);
	// Render the mesh of the object (for the depth render)
	void renderDepth(const mat4& VP, const effect& shader);

	inline bool operator==(SceneObject b){
		cout << "Test for operator== in SceneObject:" << this << " | " << &b << endl;

		if (this == &b){
			return true;
		}
		return false;
	}

};
