#version 410

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{
	// Depth is rendered to the depth part of the fbo automatically
}