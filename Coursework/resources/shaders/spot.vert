#version 440

// Model transform matrix
uniform mat4 M;
// MVP matrix
uniform mat4 MVP;

uniform bool affectedByWind;
uniform float windFactor;
uniform float totalTime;

// Incoming values
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing values
layout (location = 0) out vec3 out_position;
// Outgoing transformed normal
layout (location = 1) out vec3 transformed_normal;
// Outgoing texture coordinate
layout (location = 2) out vec2 tex_coord_out;

void main()
{	
	// Wind
	vec3 translation = vec3(0);
	if (affectedByWind) translation = vec3(sin(totalTime + (position.x + position.z) / 100.0 ) * windFactor * (normalize(position).y), 0, sin(totalTime + (position.x + position.z) / 100.0) * windFactor * (normalize(position).y));

	// Calculate screen position of vertex
	vec4 v = vec4(position + translation, 1.0);

	// Calculate screen position
	gl_Position = MVP * v;

	// Output values for the fragment shader
	out_position = (M * v).xyz;
	transformed_normal = normalize(normal);
	tex_coord_out = tex_coord_in;
}