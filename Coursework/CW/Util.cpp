/*

Grant Smith (40111906)

TODO

*/

#include <math.h>
#include <renderer.h>
#include "Util.h"

using namespace glm;

using namespace Util;

	geometry arrowGeom;

	void Util::init(){
		arrowGeom = loadModel("arrow\\arrow.obj");
	}


	// Loads in a model and returns it
	geometry Util::loadModel(string modelName){ return geometry("..\\resources\\models\\" + modelName); }

	//Returns the magnitude vec3 v
	float Util::magnitude(vec3 v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}

	//Checks if vec3 v is a zero vector
	bool Util::isZeroVec3(vec3 v){
		return v.x == 0.0 && v.y == 0.0 && v.z == 0.0;
	}
	

	// Returns the result of this vector rotated by degrees around axis
	vec3 Util::rotate(vec3 v, vec3 axis, float degrees){
		// Convert the input degrees to radians
		float angleInRad = radians(degrees);
		// Calculate the rotation around each axis
		float a = axis.x * angleInRad;
		float b = axis.y * angleInRad;
		float c = axis.z * angleInRad;

		// Create the rotation matrix using the angles a, b, and c
		mat4 rotationMat = mat4(
			cos(b)*cos(c), cos(c)*sin(a)*sin(b) - cos(a)*sin(c), cos(a)*cos(c)*sin(b) + sin(a)*sin(c), 0.0,
			cos(b)*sin(c), cos(a)*cos(c) + sin(a)*sin(b)*sin(c), -cos(c)*sin(a) + cos(a)*sin(b)*sin(c), 0.0,
			-sin(b), cos(b)*sin(a), cos(a)*cos(b), 0.0,
			0.0, 0.0, 0.0, 1.0
			);

		//Rotates the vector using a vec4
		vec4 rotatedVec3 = vec4(v.x, v.y, v.z, 1.0) * rotationMat;
		//Converts the vec4 back to a vec3 and returns it
		return vec3(rotatedVec3.x, rotatedVec3.y, rotatedVec3.z);
	}

	//TODO
	mat4 Util::rotationMat4(vec3 axis, float angle){
		float s = sin(angle);
		float c = cos(angle);
		float t = 1 - c;

		vec3 ax = normalize(axis);

		float x = ax.x;
		float y = ax.y;
		float z = ax.z;

		mat4 rotate(t*x*x + c, t*x*y - s*z, t*x*z + s*y, 0,
			t*y*x + s*z, t*y*y + c, t*y*z - s*x, 0,
			t*z*x - s*y, t*z*y + s*x, t*z*z + c, 0,
			0, 0, 0, 1);

		return rotate;
	}

	vec4 Util::vec3ToVec4(vec3 v){
		return vec4(v.x, v.y, v.z, 1.0);
	}
	vec3 Util::vec4ToVec3(vec4 v){
		return vec3(v.x / v.w, v.y / v.w, v.z / v.w);
	}

	string Util::vec3ToString(vec3 v){
		return to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z);
	}

	void Util::renderArrow(vec3 start, vec3 end, float length, float radius, mat4& PV, effect currentEffect){
		// Calculate the rotation of the arrow
		float m = sqrt(2.f + 2.f * dot(vec3(1, 0, 0), normalize(end - start)));
		vec3 w = (1.f / m) * cross(vec3(1, 0, 0), normalize(end - start));
		quat q = quat(0.5f * m, w.x, w.y, w.z);

		// Create a transform to store the rotation, translation and scale of the arrow
		graphics_framework::transform t;
		t.rotate(q);
		t.translate(start);
		t.scale = vec3(length, radius, radius);

		// Pass the transform matrix to the shader
		mat4 MVP = PV * t.get_transform_matrix();
		glUniformMatrix4fv(
			currentEffect.get_uniform_location("MVP"), // Location of uniform
			1, // Number of values - 1 mat4
			GL_FALSE, // Transpose the matrix?
			value_ptr(MVP)); // Pointer to matrix data

		// Render the arrow
		renderer::render(arrowGeom);
	}


	vec3 Util::translationFromMat4(mat4& m){
		return vec3(m[3][0], m[3][1], m[3][2]);
	}

	mat4 Util::translationMat4(vec3& v){
		return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			v.x,v.y,v.z, 1
			);
	}
