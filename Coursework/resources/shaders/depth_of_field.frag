#version 440

uniform sampler2D tex; // Texture of the captured scene
uniform sampler2D depth; // Texture of the depth of the captured scene
uniform float screenWidth; // Width of the screen
uniform float screenHeight; // Height of the screen

#define PI 3.14159265

// Near and far planes of camera
uniform float near;
uniform float far;

//uniform variables from external script

uniform float focalLength; //focal length in mm
uniform float fstop; //f-stop value
uniform bool dofDebug; //show debug focus point and focal range (red = focal point, green = focal range)

// Unit in texture space
vec2 texel = vec2(1.0 / screenWidth, 1.0 / screenHeight);

uniform int samples; // Number of samples on the first ring
uniform int rings; // Number of rings

float circleOfConfusion = 0.125f; // The Size of the circle of confusion
vec2 focus = vec2(0.5, 0.5); // The point on the screen to auto focus to
float threshold = 0.5f; // Threshold to define what is and isn't a highlight
float gain = 1.5f; // Gain applied to highlights
float bias = 0.5f; // The Bokeh edge bias
float fringe = 1.25f; // Bokeh chromatic fringing
float noiseAmount = 0.00001f; // Amount that the noise should dither
float depthBlurSize = 1.25f; // The size of the blurring applied to the depth


// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 out_colour;



// Linearize the depth using the near and far
float linearize(float depthVal)
{
	return -far * near / (depthVal * (far - near) - far);
}

// Get the depth value from the depth texture and then linearize it
float readDepth(in vec2 coord)
{
	if (coord.x<0.0 || coord.y<0.0) return 1.0;
	return linearize(texture2D(depth, coord).x);
}

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

void main()
{
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
	vec3 col = texture2D(tex, tex_coord).rgb;
	for (int i = 1; i <= rings; i ++)
	{
		// Increment the amount of samples relative to the current ring number
		ringsamples = i * samples;
		for (int j = 0; j < ringsamples; j ++)
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
	out_colour.rgb = col;
	out_colour.a = 1.0;
}