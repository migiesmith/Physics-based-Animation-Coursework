/*

	Grant Smith (40111906)

	Stores the transform and mesh of the object for the scene.
	Also stores the parent and child of itself so that sceneobjects can be attatched to eachother

*/

#pragma once

#include <texture.h>
#include <mesh.h>
#include <renderer.h>

#include <list>
#include <iostream>

using namespace std;
using namespace graphics_framework;
using namespace glm;

class SceneObject
{
private:
	texture *_texture; // This objects texture
	mesh _mesh; // This objects mesh
	vector<SceneObject> _children; // A list of children of this object

	// The transform of this object before a get_transform call (used to check if the transform was changed)
	graphics_framework::transform _origionalLocalTransform;

	// The transform of this object (excluding parent transforms)
	graphics_framework::transform _localTransform;

	//Only used to check if the parent's transform has changed
	graphics_framework::transform _parentTransform;

public:

	SceneObject *_parent = NULL; // The parent of this object
	texture *_normal = NULL; // The normal map of this object

	float windFactor = 0.0f; // How much this object is affected by wind
	vec3 orbitRotation = vec3(); // Stores what direction (and speed) that the object will orbit around it's origin

	SceneObject();

	// Creates a mesh object with the provided geometry
	SceneObject(geometry &geom) : _mesh(geom){ }
	SceneObject(mesh &m) : _mesh((geometry)m.get_geometry()){ }
	// Creates a mesh object with the provided geometry and material
	SceneObject(geometry &geom, material &mat) : _mesh(geom, mat){ }
	// Default copy constructor and assignment operator
	SceneObject(const SceneObject &other) : _mesh((geometry)other.get_geometry()) {
		_texture = other._texture;
		_origionalLocalTransform = other._origionalLocalTransform;
		_localTransform = other._localTransform;
		_normal = other._normal;
		_parentTransform = graphics_framework::transform();
	}

	SceneObject& operator=(const SceneObject &rhs) = default;
	// Destroys the mesh object
	~SceneObject();

	// Adds a child to the list of children
	void add_child(SceneObject child){
		_children.push_back(child);
	}

	mesh get_mesh(){ return _mesh; }

	// Removes a child from the list of children
	void remove_child(SceneObject child){
		vector<SceneObject>::iterator position = find(_children.begin(), _children.end(), child);
		if (position != _children.end())
			_children.erase(position);
	}

	// Returns a pointer to the children of this SceneObject
	vector<SceneObject> *get_children(){
		return &_children;
	}
	
	// Gets the transform object for the mesh
	graphics_framework::transform& get_transform() { _origionalLocalTransform = _localTransform; return _localTransform; }
	// Gets the transform object for the mesh (including parent transform)
	graphics_framework::transform& get_transform_with_parent() { return _mesh.get_transform(); }

	/*
		Methods used the communicate with the mesh part of this SceneObject
	*/
	// Gets the geometry object for the mesh
	const geometry& get_geometry() const { return _mesh.get_geometry(); }
	// Sets the geometry object for the mesh
	void set_geometry(const geometry &value) { _mesh.set_geometry(value); }
	// Gets the material object for the mesh
	material& get_material() { return _mesh.get_material(); }
	// Sets the material object for the mesh
	void set_material(const material &value) { _mesh.set_material(value); }
	// Gets the minimal point of the AABB defining the mesh
	glm::vec3 get_minimal() const { return _mesh.get_minimal(); }
	// Gets the maximal point of the AABB defining the mes
	glm::vec3 get_maximal() const { return _mesh.get_maximal(); }

	void set_texture(texture &value) { _texture = &value; }
	texture get_texture() { return *_texture; }
	void set_normal_texture(texture &value) { _normal = &value; }

	// Sets all of the material values 
	void set_material(glm::vec4 &emissive, glm::vec4 &diffuse, glm::vec4 &specular, float shininess){
		_mesh.get_material().set_emissive(emissive);
		_mesh.get_material().set_diffuse(diffuse);
		_mesh.get_material().set_specular(specular);
		_mesh.get_material().set_shininess(shininess);
	}

	void update(float delta_time);

	// Render the mesh of the object
	void render(){
		renderer::render(get_mesh());
	}

	inline bool operator==(SceneObject b){
		cout << "Test for operator== in SceneObject:" << this << " | " << &b << endl;

		if (this == &b){
			return true;
		}
		return false;
	}

};
