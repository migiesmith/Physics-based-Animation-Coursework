#version 440

uniform sampler2D tex; // Texture of the captured scene
uniform sampler2D depth; // Texture of the depth of the captured scene
uniform float screenWidth; // Width of the screen
uniform float screenHeight; // Height of the screen

#define PI 3.14159265

// Near and far planes of camera
uniform float near;
uniform float far;

uniform float aoRadius; // Radius of the Ambient Occlusion (used to calculate displacements for depth comparisons)
uniform int samples; // Number of samples on the first ring
uniform int rings; // Number of rings
uniform float lumInfluence; //how much luminance affects occlusion
uniform float selfShadowReduc; //self-shadowing reduction
uniform float gaussDisplace; //gauss bell center

uniform bool useNoise = false; // Use Noise or just a pattern to smooth banding
uniform bool renderOnlyAO = false; // Render AO + Colour or just AO


// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 out_colour;



// Generate a noise value for dithering
vec2 getRandom(in vec2 coord)
{
	float noiseX;
	float noiseY;

	// Return noise depending on whether intensive noise is on or not
	if (useNoise)
	{
		// Get noisey noise using coord
		noiseX = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453), 0.0, 1.0)*2.0 - 1.0;
		noiseY = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233)*2.0)) * 43758.5453), 0.0, 1.0)*2.0 - 1.0;
		// More intensive
	}
	else{
		// Get get not very noisey noise using coord and the screen dimensions
		noiseX = ((fract(1.0 - coord.s*(screenWidth / 2.0))*0.25) + (fract(coord.t*(screenHeight / 2.0))*0.75))*2.0 - 1.0;
		noiseY = ((fract(1.0 - coord.s*(screenWidth / 2.0))*0.75) + (fract(coord.t*(screenHeight / 2.0))*0.25))*2.0 - 1.0;
		// Less intensive
	}
	return vec2(noiseX, noiseY)*0.001;
}

// Get the depth value from the depth texture
float readDepth(in vec2 coord)
{
	if (coord.x<0.0 || coord.y<0.0) return 1.0;
	return (2.0 * near) / (far + near - texture2D(depth, coord).x * (far - near));
}

// Compare the depth values and return a blur of the two
float compareDepths(in float depth1, in float depth2, inout int far)
{
	float gaussArea = 2.0; // The width of the gaussian bell 
	float diff = (depth1 - depth2)*100.0; // The depth difference between 0 and 100

	//reduce bell width to avoid self-shadowing 
	if (diff<gaussDisplace)
	{
		/*
			If the depth difference is less than the gaussian area then set the gaussian area to selfShadowReduc
			to prevent self shadowing
		*/
		gaussArea = selfShadowReduc;
	}
	else{
		// If the depth difference is greater than the gaussian area then set the far to 1
		far = 1;
	}

	// Calculate the new far value using a gaussian bell
	float gauss = pow(2.7182, -2.0*(diff - gaussDisplace)*(diff - gaussDisplace) / (gaussArea*gaussArea));
	return gauss;
}

// Calculates the Ambient Occlusion at a point using the depth and a displacement (on x and y)
float calcAmbientOcclusion(float depth, float dX, float dY)
{
	/*
		Get the amount of displacement using the inverse of the depth of the pixel multiplied by the ambient occlusion radius
	*/
	float depthDisplacement = (1.0 - depth)*aoRadius;

	// Store the parts of the output
	float result = 0.0;
	float partialResult = 0.0;

	// Create displaced texture coordinates
	vec2 coord = vec2(tex_coord.x + dX*depthDisplacement, tex_coord.y + dY*depthDisplacement);
	vec2 coord2 = vec2(tex_coord.x - dX*depthDisplacement, tex_coord.y - dY*depthDisplacement);

	int far = 0;
	// Compare depth values and get a new far value
	result = compareDepths(depth, readDepth(coord), far);
	
	if (far > 0)// Extrapolate the depth
	{
		partialResult = compareDepths(readDepth(coord2), depth, far);
		result += (1.0 - result)*partialResult;
	}

	return result;
}

void main(void)
{
	// Generate a random point (vec2)
	vec2 noise = getRandom(tex_coord);
	// Read the depth of the current pixel
	float depth = readDepth(tex_coord);

	// Sample displacement scale for x and y
	float x = (1.0 / screenWidth) / clamp(depth, 0.25, 1.0) + (noise.x*(1.0 - noise.x));
	float y = (1.0 / screenHeight) / clamp(depth, 0.25, 1.0) + (noise.y*(1.0 - noise.y));

	float ao; // Store the ambient occlusion value
	float sampleSum; // Store the number of samples made

	int ringsamples; // Store the samples per ring

	// Calculate Ambient Occlusion and blur
	for (int i = 1; i <= rings; i++)
	{
		// Increment the amount of samples relative to the current ring number
		ringsamples = i * samples;
		for (int j = 0; j < ringsamples; j++)
		{
			float step = PI*2.0 / float(ringsamples);
			float kernelDisplaceX = (cos(float(j)*step)*float(i));
			float kernelDisplaceY = (sin(float(j)*step)*float(i));
			ao += calcAmbientOcclusion(depth, kernelDisplaceX*x, kernelDisplaceY*y);
			sampleSum += 1.0;
		}
	}

	// Divide the Ambient Occlusion by the Number of samples to get an average
	ao /= sampleSum;
	// Invert the Ambient Occlusion
	ao = 1.0 - ao;

	vec3 colour = texture2D(tex, tex_coord).rgb;

	// Get the luminance by taking the dot of the pixel and the luminance coefficient
	vec3 luminance = vec3(dot(colour.rgb, vec3(0.299, 0.587, 0.114))) * lumInfluence;

	if (renderOnlyAO){ // Render Ambient Occlusion only
		out_colour = vec4(vec3(mix(vec3(ao), vec3(1.0), luminance)), 1.0);
	}
	else{ // Render Ambient Occlusion + Colour
		out_colour = vec4(vec3(colour*mix(vec3(ao), vec3(1.0), luminance)), 1.0);
	}

}