

#include "Util.h"

using namespace glm;

using namespace Util;

	geometry arrowGeom;
	geometry planeGeom;
	material utilMaterial;

	// Initialise the util class
	void Util::init(){
		arrowGeom = loadModel("primitives\\arrow2.obj");
		planeGeom = loadModel("primitives\\plane.obj");

		utilMaterial.set_emissive(vec4(0.8,0.8,0.8, 1));
		utilMaterial.set_diffuse(vec4(0.9, 0.9, 0.9, 1));
		utilMaterial.set_specular(vec4(1, 1, 1, 1));
		utilMaterial.set_shininess(5.0f);
	}


	// Loads in a model and returns it
	geometry Util::loadModel(string modelName){ return geometry("..\\resources\\models\\" + modelName); }

	// Loads in a texture and returns it
	texture Util::loadTexture(string textureName, bool mipmaps, bool antisoptrics){ return texture("..\\resources\\textures\\" + textureName, mipmaps, antisoptrics); }

	// Loads in a texture and returns it
	texture Util::loadTexture(string textureName){ return loadTexture(textureName, false, false); }

	// Returns the magnitude vec3 v
	float Util::magnitude(const vec3& v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}

	// Check if vec3 v is a zero vector (0,0,0)
	bool Util::isZeroVec3(const vec3& v){
		return v.x == 0.0 && v.y == 0.0 && v.z == 0.0;
	}

	// Checks if v0 and v1 are equal
	bool Util::equals(const vec3& v0, const vec3& v1){
		return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
	}

	// Returns the result of this vector rotated by degrees around axis
	vec3 Util::rotate(const vec3& v, const vec3& axis, float degrees){
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

	// Creates a rotation matrix from an axis and an angle
	mat4 Util::rotationMat4(vec3& axis, float angle){
		float s = sin(angle);
		float c = cos(angle);
		float t = 1.0f - c;

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

	// Returns a vec4 representing vec3 v
	vec4 Util::vec3ToVec4(const vec3& v){
		return vec4(v.x, v.y, v.z, 1.0);
	}

	// Returns a vec3 representing vec4 v
	vec3 Util::vec4ToVec3(const vec4& v){
		return vec3(v.x / v.w, v.y / v.w, v.z / v.w);
	}

	// Convert vec3 v to a string
	string Util::vec3ToString(const vec3& v){
		return to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z);
	}

	// Render an arrow
	void Util::renderArrow(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect){
		// Set up the rendering
		setUpModelRendering(start, end, length, radius, PV, currentEffect);
		// Render the arrow
		renderer::render(arrowGeom);
	}

	// Render another model like the arrow is rendered (used for ik rendering)
	void Util::renderIKModel(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect, const geometry& geom){
		// Set up the rendering
		setUpModelRendering(start, end, length, radius, PV, currentEffect);
		// Render the model
		renderer::render(geom);
	}


	// Set up model rendering (for renderArrow and renderIKModel
	void Util::setUpModelRendering(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect){
		// Create a transform to store the rotation, translation and scale of the arrow
		graphics_framework::transform t;

		if (abs(dot(vec3(1, 0, 0), normalize(end - start))) != 1.0f){
			// Calculate the rotation of the arrow
			float m = sqrt(2.f + 2.f * dot(vec3(1, 0, 0), normalize(end - start)));
			vec3 w = (1.f / m) * cross(vec3(1, 0, 0), normalize(end - start));
			quat q = quat(0.5f * m, w.x, w.y, w.z);
			t.rotate(normalize(q));
		}

		t.scale = vec3(length, radius, radius);
		t.translate(start);


		// Pass the transform matrix to the shader
		mat4 MVP = PV * t.get_transform_matrix();
		glUniformMatrix4fv(
			currentEffect.get_uniform_location("MVP"), // Location of uniform
			1, // Number of values - 1 mat4
			GL_FALSE, // Transpose the matrix?
			value_ptr(MVP)); // Pointer to matrix data

		glUniformMatrix4fv(currentEffect.get_uniform_location("M"), 1, GL_FALSE, value_ptr(t.get_transform_matrix()));
		renderer::bind(utilMaterial, "mat");

	}

	// Return the translation part of mat4 m
	vec3 Util::translationFromMat4(const mat4& m){
		return vec3(m[3][0], m[3][1], m[3][2]);
	}

	// Create a translation matrix from v
	mat4 Util::translationMat4(const vec3& v){
		return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			v.x,v.y,v.z, 1
			);
	}

	// Returns a vec3 representing the rotation of quat q as vec3(roll, pitch, yaw)
	vec3 Util::rollPitchYawFromQuat(const quat& q){
		vec3 v = vec3(0, 0, 0);
		v.x = atan2(2 * q.y*q.w - 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z); // Roll
		v.y = atan2(2 * q.x*q.w - 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z); // Pitch
		v.z = asin(2 * q.x*q.y + 2 * q.z*q.w); // Yaw
		return v;
	}