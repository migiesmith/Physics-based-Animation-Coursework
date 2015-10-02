#version 440

// Model view projection matrix
uniform mat4 MVP;
uniform float near;
uniform float far;
uniform float totalTime;
uniform float windFactor;

layout (location = 0) in vec3 position;

void main()
{
	// Wind
	vec3 translation = vec3(0);
	if (windFactor != 0.0f) translation = vec3(1, 0, 1) * sin(totalTime + (position.x + position.z) / 100.0) * windFactor * (normalize(position).y);

	// Calculate screen position of vertex
	vec4 v = vec4(position + translation, 1.0);
	gl_Position = MVP * v;
}