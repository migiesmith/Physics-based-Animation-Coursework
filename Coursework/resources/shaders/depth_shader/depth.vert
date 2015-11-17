#version 410

// Model view projection matrix
uniform mat4 MVP;
uniform float near;
uniform float far;
uniform float totalTime;
uniform float windFactor;

layout (location = 0) in vec3 position;

void main()
{
	// Calculate screen position of vertex
	vec4 v = vec4(position, 1.0);
	gl_Position = MVP * v;
}