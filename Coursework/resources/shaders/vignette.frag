#version 440

// Sampler used to get texture colour
uniform sampler2D tex;

uniform float outerRadius, innerRadius, intensity;

// Incoming texture coordinate
layout (location = 0) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{

	//Get the texture coordinate relative to the centre of the screen
	vec2 vignette_coords = tex_coord - vec2(0.5f);
	float vignette = smoothstep(outerRadius, innerRadius, length(vignette_coords));

	//Mix the vignette value withe the image colour
	out_colour = texture2D(tex, tex_coord);
	out_colour.rgb = mix(out_colour.rgb, out_colour.rgb * vignette, intensity);

}