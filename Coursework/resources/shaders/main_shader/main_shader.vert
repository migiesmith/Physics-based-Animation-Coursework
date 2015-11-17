#version 410

// Model view projection matrix
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 lightVP;
uniform mat4 depthBias;

uniform vec3 ambientLightDir;
uniform float totalTime;
uniform vec3 cameraPos;


layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout (location = 10) in vec2 tex_coord_in;

layout (location = 0) out vec2 tex_coord_out;

layout (location = 2) out vec3 norm;

layout(location = 11) out vec4 shadowCoord;


void main()
{
	norm = normalize(normal);

	// Calculate screen position of vertex
	vec4 v = vec4(position, 1.0);
	gl_Position = MVP * v;

	shadowCoord = (depthBias*(lightVP * M)) * v;

	// Output texture coordinate to fragement shader
	tex_coord_out = tex_coord_in;
}