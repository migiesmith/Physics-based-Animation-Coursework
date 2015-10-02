#version 330

uniform sampler2D tex; // Texture of the captured scene
uniform sampler2D depth; // Texture of the depth of the captured scene
uniform sampler2D dirt;
uniform sampler2D prevTex;

uniform float screenWidth; // Width of the screen
uniform float screenHeight; // Height of the screen

#define PI 3.14159265

// Near and far planes of camera
uniform float near;
uniform float far;

//----------------------------------------------------------------------------------

// SSAO params start
uniform float aoRadius; // Radius of the Ambient Occlusion (used to calculate displacements for depth comparisons)
uniform int samples; // Number of samples on the first ring
uniform int rings; // Number of rings
uniform float lumInfluence; //how much luminance affects occlusion
uniform float selfShadowReduc; //self-shadowing reduction
uniform float gaussDisplace; //gauss bell center

uniform bool useNoise = false; // Use Noise or just a pattern to smooth banding
uniform bool renderOnlyAO = false; // Render AO + Colour or just AO
// SSAO params end

//----------------------------------------------------------------------------------

// Lens Flare params start
uniform int ghostCount; // number of ghost samples
uniform float ghostDispersal; // dispersion factor
// Lens Flare params end

//----------------------------------------------------------------------------------

// Vignette params start
uniform float outerRadius, innerRadius, intensity;
// Vignette params end

//----------------------------------------------------------------------------------

// Depth of Field params start

uniform float focalLength; //focal length in mm
uniform float fstop; //f-stop value
uniform bool dofDebug; //show debug focus point and focal range (red = focal point, green = focal range)

// Unit in texture space
vec2 texel = vec2(1.0 / screenWidth, 1.0 / screenHeight);

float circleOfConfusion = 0.125f; // The Size of the circle of confusion
vec2 focus = vec2(0.5, 0.5); // The point on the screen to auto focus to
float threshold = 0.5f; // Threshold to define what is and isn't a highlight
float gain = 1.5f; // Gain applied to highlights
float bias = 0.5f; // The Bokeh edge bias
float fringe = 1.25f; // Bokeh chromatic fringing
float noiseAmount = 0.00001f; // Amount that the noise should dither
float depthBlurSize = 1.25f; // The size of the blurring applied to the depth

// Depth of Field params end

//----------------------------------------------------------------------------------

// Post processing pass toggles start
uniform bool ssaoEnabled;
uniform bool glowEnabled;
uniform bool lensFlareEnabled;
uniform bool motionBlurEnabled;
uniform bool greyscaleEnabled;
uniform bool vignetteEnabled;
uniform bool dofEnabled;
// Post processing pass toggles end

//----------------------------------------------------------------------------------

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 out_colour;

//----------------------------------------------------------------------------------

// Post processing helper functions start

// Linearize the depth using the near and far
float linearize(float depthVal)
{
	return -far * near / (depthVal * (far - near) - far);
}

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
	return (2.0 * near) / (far + near - texture2D(depth, coord).x * (far - near));//linearize(texture2D(depth, coord).x);//
}


//dof helper start

// Blurs the depth value to smooth out the depth of field
float bDepth(vec2 coords)
{
	float outputDepth = 0.0;
	float kernel[9];
	vec2 offset[9];

	vec2 wh = vec2(texel.x, texel.y) * depthBlurSize;

	// Set up the offset array
	offset[0] = vec2(-wh.x, -wh.y);
	offset[1] = vec2(0.0, -wh.y);
	offset[2] = vec2(wh.x - wh.y);
	offset[3] = vec2(-wh.x, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(wh.x, 0.0);
	offset[6] = vec2(-wh.x, wh.y);
	offset[7] = vec2(0.0, wh.y);
	offset[8] = vec2(wh.x, wh.y);

	// Set up the kernel array
	kernel[0] = 1.0 / 16.0;   kernel[3] = 2.0 / 16.0;   kernel[6] = 1.0 / 16.0;
	kernel[1] = 2.0 / 16.0;   kernel[4] = 4.0 / 16.0;   kernel[7] = 2.0 / 16.0;
	kernel[2] = 1.0 / 16.0;   kernel[5] = 2.0 / 16.0;   kernel[8] = 1.0 / 16.0;

	/*
	Loop 9 times, getting the pixel at coord + the ith offset, multiply it by the kernel value
	at the ith value and then add it to the outputDepth
	*/
	for (int i = 0; i<9; i++)
	{
		float tmp = texture2D(depth, coords + offset[i]).r;
		outputDepth += tmp * kernel[i];
	}

	return outputDepth;
}

// Get a sample using coord and apply chromatic abberation
vec3 colour(vec2 coord, float blur)
{
	// Init a vec3 to store the colour values
	vec3 col = vec3(0.0f);

	// Get the rgb values from the texture and apply blurring
	col.r = texture(tex, coord + vec2(0.0f, 1.0f)*texel*fringe*blur).r;
	col.g = texture(tex, coord + vec2(-0.866f, -0.5f)*texel*fringe*blur).g;
	col.b = texture(tex, coord + vec2(0.866f, -0.5f)*texel*fringe*blur).b;

	// Use the luminance of the pixel to apply blurring
	vec3 lumcoeff = vec3(0.299, 0.587, 0.114);
	float lum = dot(col.rgb, lumcoeff);
	float thresh = max((lum - threshold)*gain, 0.0);
	return col + mix(vec3(0.0f), col, thresh*blur);
}

// Generate a noise value for dithering
vec2 rand(in vec2 coord)
{
	// Get noise using coord
	float noiseX = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453), 0.0, 1.0)*2.0 - 1.0;
	float noiseY = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233)*2.0)) * 43758.5453), 0.0, 1.0)*2.0 - 1.0;

	// Return the generated noise
	return vec2(noiseX, noiseY);
}

// Render the depth of field in debug to show the change in dof intensity
vec3 debugFocus(vec3 col, float blur, float depthVal)
{
	float edge = 0.002*depthVal; //distance based edge smoothing
	float outOfFocus = clamp(smoothstep(0.0, edge, blur), 0.0, 1.0);
	float inFocus = clamp(smoothstep(1.0 - edge, 1.0, blur), 0.0, 1.0);

	// Add red to the output colour depending on how out of focus the pixel is
	col = mix(col, vec3(1.0, 0.0, 0.0), (1.0 - outOfFocus)*0.6);
	// Add blue to the output colour depending on how in focus the pixel is
	col = mix(col, vec3(0.0, 0.0, 1.0), ((1.0 - inFocus) - (1.0 - outOfFocus))*0.2);

	return col;
}
// dof helper end

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

// Post processing helper functions end

//----------------------------------------------------------------------------------

// Functions for post processing passes start

vec4 ssaoPass(vec4 texColor){
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
	
	// Get the luminance by taking the dot of the pixel and the luminance coefficient
	vec3 luminance = vec3(dot(texColor.rgb, vec3(0.299, 0.587, 0.114))) * lumInfluence;

	if (renderOnlyAO){ // Render Ambient Occlusion only
		return vec4(vec3(mix(vec3(ao), vec3(1.0), luminance)), 1.0);
	}
	else{ // Render Ambient Occlusion + Colour
		return vec4(vec3(texColor.rgb*mix(vec3(ao), vec3(1.0), luminance)), 1.0);
	}
}

vec4 glowPass(vec4 texColour){
	vec3 luminance = vec3(dot(texColour.rgb, vec3(0.299, 0.587, 0.114)));

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
	return blur / 25.0f + texColour;
}

vec4 lensFlarePass(vec4 texColour){
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
	return texColour + (result * texture(dirt, tex_coord)) * 0.350f;
}

vec4 motionBlurPass(vec4 texColour){
	return mix(texColour, texture(prevTex, tex_coord), 0.25f);
}

vec4 greyScalePass(vec4 texColour){
	vec3 luminance = vec3(dot(texColour.rgb, vec3(0.299, 0.587, 0.114)));
	return vec4(luminance, 1.0);
}

vec4 vignettePass(vec4 texColour){
	//Get the texture coordinate relative to the centre of the screen
	vec2 vignette_coords = tex_coord - vec2(0.5f);
	float vignette = smoothstep(outerRadius, innerRadius, length(vignette_coords));

	//Mix the vignette value withe the image colour
	return vec4(mix(texColour.rgb, texColour.rgb * vignette, intensity), 1.0); 
}

vec4 dofPass(vec4 texColour){

	// Calculate the depth at this pixel
	float depthVal = linearize(bDepth(tex_coord));

	// Calculate the depth at the focal point
	float fDepth = linearize(texture2D(depth, focus).x) * 1000.0f;

	// Calculate the depth blur factor
	float blurP0 = ((depthVal*1000.0)*focalLength) / ((depthVal*1000.0) - focalLength);
	float blurP1 = (fDepth*focalLength) / (fDepth - focalLength);
	float blurP2 = (fDepth - focalLength) / (fDepth*fstop*circleOfConfusion);

	float blur = abs(blurP0 - blurP1)*blurP2;
	blur = clamp(blur, 0.2f, 1.0f);

	// Calculate the noise for dithering
	vec2 noise = rand(tex_coord)*noiseAmount*blur;

	// Sample displacement scale for x and y
	float w = (1.0 / screenWidth)*blur + noise.x;
	float h = (1.0 / screenHeight)*blur + noise.y;


	float sampleSum; // Store the number of samples made
	int ringsamples; // Store the samples per ring

	// Calculate the final pixel colour using the initial colour and blurring with depending on the depth
	vec3 col = texColour.rgb;
	for (int i = 1; i <= rings; i++)
	{
		// Increment the amount of samples relative to the current ring number
		ringsamples = i * samples;
		for (int j = 0; j < ringsamples; j++)
		{
			float step = PI*2.0 / float(ringsamples);
			float wDisplace = (cos(float(j)*step)*float(i));
			float hDisplace = (sin(float(j)*step)*float(i));
			col += colour(tex_coord + vec2(wDisplace*w, hDisplace*h), blur);
			sampleSum += 1.0;
		}
	}
	// Divide col by the amount of samples to get the final colour
	col /= sampleSum;

	// Check if debug is on. If it is then render debug mode
	if (dofDebug)
	{
		col = debugFocus(col, blur, depthVal);
	}

	// Set the output colour
	return vec4(col, 1.0);
}

// Functions for post processing passes end

//----------------------------------------------------------------------------------

void main(void)
{
	// Read texture colour
	vec4 texColour = texture(tex, tex_coord);

	// Run shader passes depending on the passed in toggles
	if (ssaoEnabled) texColour = ssaoPass(texColour);
	if (dofEnabled) texColour = dofPass(texColour);
	if (glowEnabled) texColour = glowPass(texColour);
	if (lensFlareEnabled) texColour = lensFlarePass(texColour);
	if (motionBlurEnabled) texColour = motionBlurPass(texColour);
	if (greyscaleEnabled) texColour = greyScalePass(texColour);
	if (vignetteEnabled) texColour = vignettePass(texColour);
	
	// Set the pixel colour
	out_colour = texColour;
}



