/*

Grant Smith (40111906)

Constructor/Destructor of the SceneObject

*/
#include "SceneObject.h"


SceneObject::SceneObject()
{
}

void SceneObject::update(float delta_time){

	/*
		Checks if the orbitRotation is set the to a 0 vector
		If it isn't then apply rotation on the axis (definied by orbitRotation)
	*/
	if (orbitRotation != vec3()){
		mat4 rotationMat(1);

		// Rotate by the scale of orbit rotation
		float speed = dot(orbitRotation, normalize(orbitRotation));

		get_transform().rotate(vec3(quarter_pi<float>(), 0, 0)*delta_time);
		rotationMat = rotate(rotationMat, (quarter_pi<float>() / 2)*delta_time*speed, orbitRotation);
		get_transform().position
			= vec3(rotationMat * vec4(get_transform().position, 1.0f));
	}

	// Checks if the transform of this object has been changed
	if (_localTransform.get_transform_matrix() != _origionalLocalTransform.get_transform_matrix()){
		_mesh.get_transform() = _localTransform;
		_origionalLocalTransform = _localTransform;
	}

	// If this object has a parent
	if (_parent){

		// If the parent's transform has changed
		if (_parent->get_transform_with_parent().get_transform_matrix() != _parentTransform.get_transform_matrix()){

			//Get the current transform
			graphics_framework::transform t = get_transform();

			t.rotate(_parent->get_transform_with_parent().orientation);

			if (_parent->get_transform_with_parent().orientation != quat())
				t.position = rotate(_parent->get_transform_with_parent().orientation, t.position);

			t.translate(_parent->get_transform_with_parent().position);

			t.scale *= _parent->get_transform_with_parent().scale;

			get_transform_with_parent() = _parentTransform = t;
		}
	}

	// Checks if this object has children to update
	for (auto &child : *(get_children())){
		child.update(delta_time);
	}

}

SceneObject::~SceneObject()
{
}
