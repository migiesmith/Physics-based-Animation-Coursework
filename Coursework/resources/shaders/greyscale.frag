#version 440

// Sampler used to get texture colour
uniform sampler2D tex;

// Incoming texture coordinate
layout (location = 0) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{
	vec3 luminance = vec3(dot(texture(tex, tex_coord).rgb, vec3(0.299, 0.587, 0.114)));

	out_colour = vec4(luminance, 1.0);	
}