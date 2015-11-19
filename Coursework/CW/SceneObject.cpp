/*

Grant Smith (40111906)

Constructor/Destructor of the SceneObject

*/
#include "SceneObject.h"
#include "SPGrid.h"


SceneObject::SceneObject()
{
	velocity = vec3(0, 0, 0);
	force = vec3(0, 0, 0);
}

void SceneObject::update(float delta_time){
	if (_collider){
		_collider->update(delta_time);
		IntersectionData data = IntersectionData();
		SPGrid::getInstance().intersects(*_collider, velocity*delta_time, data);

		get_transform().position = _collider->position;
		//if (_collider->getType() == ColliderTypes::OBBCUBE)
		//	get_transform().orientation = toQuat(((CubeCollider&)_collider).rotation);
	}
}

void SceneObject::intersects(Collider& c, const vec3& velocity, IntersectionData& data){
	if (_collider)
		_collider->intersects(c, velocity, data);
}

void SceneObject::render(const mat4& VP, const effect& shader){
	mat4 M = get_transform().get_transform_matrix();
	mat4 MVP = VP * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		shader.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(MVP)); // Pointer to matrix data

	glUniformMatrix4fv(shader.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

	// Check if the SceneObject has a normal map
	if (_normal){
		// Bind the normal
		renderer::bind(*(_normal), 1);
		glUniform1i(shader.get_uniform_location("hasNormalMap"), true);
		glUniform1i(shader.get_uniform_location("normalTex"), 1);
	}
	else{
		glUniform1i(shader.get_uniform_location("hasNormalMap"), false);
	}

	// Bind and set texture
	renderer::bind(get_texture(), 0);
	glUniform1i(shader.get_uniform_location("tex"), 0);

	// Bind the SceneObject's material
	renderer::bind(get_material(), "mat");

	renderer::render(*this);
}

void SceneObject::renderDepth(const mat4& VP, const effect& shader){
	mat4 MVP = VP * get_transform().get_transform_matrix();
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		shader.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(MVP)); // Pointer to matrix data

	// Render the current SceneObject
	renderer::render(*this);
}


SceneObject::~SceneObject()
{
}
