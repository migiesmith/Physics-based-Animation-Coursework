/*

Grant Smith (40111906)

TODO

*/

#include "Util.h"

using namespace glm;

using namespace Util;

	geometry arrowGeom;
	geometry planeGeom;

	void Util::init(){
		arrowGeom = loadModel("primitives\\arrow2.obj");
		planeGeom = loadModel("primitives\\plane.obj");
	}


	// Loads in a model and returns it
	geometry Util::loadModel(string modelName){ return geometry("..\\resources\\models\\" + modelName); }

	// Loads in a texture and returns it
	texture Util::loadTexture(string textureName, bool mipmaps, bool antisoptrics){ return texture("..\\resources\\textures\\" + textureName, mipmaps, antisoptrics); }

	// Loads in a texture and returns it
	texture Util::loadTexture(string textureName){ return loadTexture(textureName, false, false); }

	//Returns the magnitude vec3 v
	float Util::magnitude(const vec3& v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}

	//Returns the magnitude vec3 v (without the sqrt)
	float Util::sqdMagnitude(const vec3& v){
		return pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);
	}

	//Checks if vec3 v is a zero vector
	bool Util::isZeroVec3(vec3 v){
		return v.x == 0.0 && v.y == 0.0 && v.z == 0.0;
	}

	bool Util::equals(vec3 v0, vec3 v1){
		return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
	}

	float Util::ToAxisAngle(const quat& q, vec3& v)
	{
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		float sqrLength = q.x*q.x + q.y*q.y + q.z*q.z;
		if (sqrLength > 0.0f)
		{
			float invLength = 1.0f / std::sqrt(sqrLength);

			v.x = q.x*invLength;
			v.y = q.y*invLength;
			v.z = q.z*invLength;

			return 2.f*std::acos(q.w);
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do.
			v.x = 1.0f;
			v.y = 0.0f;
			v.z = 0.0f;

			return 0.f;
		}
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

	vec4 Util::vec3ToVec4(vec3 v){
		return vec4(v.x, v.y, v.z, 1.0);
	}
	vec3 Util::vec4ToVec3(vec4 v){
		return vec3(v.x / v.w, v.y / v.w, v.z / v.w);
	}

	string Util::vec3ToString(vec3 v){
		return to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z);
	}

	void Util::renderArrow(const vec3& start, const vec3& end, const float length, const float radius, const  mat4& PV, effect& currentEffect){
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

		// Render the arrow
		renderer::render(arrowGeom);
	}

	void Util::renderPlane(const vec3& pos, const  vec3 scale, const  mat4& VP, effect& currentEffect){
		// Create a transform to store the rotation, translation and scale of the arrow
		graphics_framework::transform t;

		t.scale = scale;
		t.translate(pos);

		// Pass the transform matrix to the shader
		mat4 MVP = VP * t.get_transform_matrix();
		glUniformMatrix4fv(
			currentEffect.get_uniform_location("MVP"), // Location of uniform
			1, // Number of values - 1 mat4
			GL_FALSE, // Transpose the matrix?
			value_ptr(MVP)); // Pointer to matrix data

		// Render the arrow
		renderer::render(planeGeom);
	}

	mat4 Util::mult(const mat4& a, const mat4& b){
		mat4 newMatrix;
		for (int x = 0; x < 4; x++) { // Loop through rows
			for (int y = 0; y < 4; y++) { // Loop through columns
				newMatrix[x][y] = 0.0f;
				for (int z = 0; z < 4; z++) { // Added elements
					newMatrix[x][y] += a[x][z] * b[z][y];
				}
			}
		}
		return newMatrix;
	}

	quat Util::mult(quat q0, quat q1){
		float s0 = q0.w;
		float s1 = q1.w;
		vec3 v0 = vec3(q0.x, q0.y, q0.z);
		vec3 v1 = vec3(q1.x, q1.y, q1.z);
		return quat(s0*s1 - dot(v0, v1), cross(v0, v1) + s0*v1 + s1*v0);
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

	mat4 Util::quatToMat4(quat& inQuat){
		quat q = normalize(inQuat);
		return mat4(
			1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z, 2.0f*q.x*q.y - 2.0f*q.z*q.w, 2.0f*q.x*q.z + 2.0f*q.y*q.w, 0.0f,
			2.0f*q.x*q.y + 2.0f*q.z*q.w, 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z, 2.0f*q.y*q.z - 2.0f*q.x*q.w, 0.0f,
			2.0f*q.x*q.z - 2.0f*q.y*q.w, 2.0f*q.y*q.z + 2.0f*q.x*q.w, 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Returns a vec3 representing the rotation as vec3(roll, pitch, yaw)
	vec3 Util::rollPitchYawFromQuat(const quat& q){
		vec3 v = vec3(0, 0, 0);
		v.x = atan2(2 * q.y*q.w - 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z); // Roll
		v.y = atan2(2 * q.x*q.w - 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z); // Pitch
		v.z = asin(2 * q.x*q.y + 2 * q.z*q.w); // Yaw
		return v;
	}

	pair<vec3, vec3> Util::rayFromScreen(vec3& camPos, vec2& clickPos){

		return pair<vec3, vec3>(vec3(0,0,0), vec3(0,0,0));
	}