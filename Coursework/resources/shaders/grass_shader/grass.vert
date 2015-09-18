#version 440

// Model view projection matrix
uniform mat4 MVP;

uniform float count;

layout (location = 0) in vec3 position;
layout(location = 1) in vec4 in_colour;

layout(location = 1) out vec4 out_colour;
out vec3 worldPos;


void main()
{
	// Calculate screen position of vertex
	gl_Position = MVP * vec4(position, 1.0);
	
	worldPos = position;
}