#version 410

// Model view projection matrix
uniform mat4 MVP;
uniform mat4 M;

uniform vec3 ambientLightDir;
uniform float totalTime;
uniform vec3 cameraPos;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout (location = 10) in vec2 tex_coord_in;

layout (location = 0) out vec2 tex_coord_out;

layout (location = 2) out vec3 norm;


void main()
{
	norm = normalize((vec4(normal, 1.0) * M).xyz);

	// Calculate screen position of vertex
	vec4 v = vec4(position, 1.0);
	gl_Position = MVP * v;

	// Output texture coordinate to fragement shader
	tex_coord_out = tex_coord_in;
}