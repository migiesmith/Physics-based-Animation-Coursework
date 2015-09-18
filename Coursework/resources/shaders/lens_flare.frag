#version 440

// Sampler used to get texture colour
uniform sampler2D tex;
uniform sampler2D dirt;
uniform float screenWidth; // Width of the screen
uniform float screenHeight; // Height of the screen

uniform int ghostCount; // number of ghost samples
uniform float ghostDispersal; // dispersion factor

// Incoming texture coordinate
layout (location = 0) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 out_colour;


void main()
{

	vec2 invCoords = -tex_coord + vec2(1.0f);

	// Ghost vector relative to image centre
	vec2 ghostVec = (vec2(0.5f) - invCoords) * ghostDispersal;

	// Temporary store for the final colour
	vec4 result = vec4(0.0);

	// Apply lens ghosts using light parts of the texture at the inverse coord
	for (int i = 0; i < ghostCount; ++i) {
		// Use fract to force texture repeating (similar effect to GL_REPEAT)
		vec2 offset = fract(invCoords + ghostVec * float(i));

		float weight = length(vec2(0.5f) - offset) / length(vec2(0.5));
		weight = pow(1.0f - weight, 10.0f);

		result += texture(tex, offset) * weight;
	}

	// Apply a halo using light parts of the texture relative to the inverse coord
	vec2 haloVec = normalize(ghostVec) * 0.5f;
	float weight = length(vec2(0.5f) - fract(invCoords + haloVec)) / length(vec2(0.5));
	weight = pow(1.0f - weight, 5.0f);
	result += texture(tex, invCoords + haloVec) * weight;
	
	// Merge the actual texture colour with the result multiplied by the dirt texture
	out_colour = texture(tex, tex_coord) + (result * texture(dirt, tex_coord)) * 0.350f;

}