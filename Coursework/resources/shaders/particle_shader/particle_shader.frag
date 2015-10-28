#version 440

// Sampler used to get texture colour
uniform sampler2D tex;
uniform vec4 colour;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 out_colour;

void main()
{
	// Set the outgoing colour to the colour on the texture at the location tex_coord
	out_colour = texture(tex, tex_coord) * colour;
}