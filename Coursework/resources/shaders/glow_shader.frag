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

	// Create a blurred pixel using the pixels next to it
	vec4 blur = vec4(0);
	for (int i = -2; i < 2; i++){
		for (int j = -2; j < 2; j++){
			blur += texture(tex, tex_coord + vec2(i, j) *0.0025f)*(luminance.r*1.75f);
		}
	}

	/*
		Merge the blur with the pixel colour
		Reduce the intensity of the pixel colour so that the scene isn't too bright
	*/
	out_colour = blur / 25.0f + texture(tex, tex_coord);
}