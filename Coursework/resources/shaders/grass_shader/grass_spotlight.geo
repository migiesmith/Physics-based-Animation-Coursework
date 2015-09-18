#version 440

uniform mat4 MVP;
uniform sampler2D heightTex;
uniform float count;
uniform vec3 camPos;
uniform vec3 eyeDir;

uniform float windFactor;
uniform float totalTime;

layout(points) in;
layout(triangle_strip, max_vertices = 45) out;

// Incoming
in vec3 worldPos[];

// Outgoing
layout(location = 2) out vec3 wPosition;
layout(location = 0) out vec2 tex_coord;


// Points for tall blades of grass
vec4 tallPoints[] = {
	vec4(0, 0, 0.1, 1),
	vec4(0.3, 0, 0.1, 1),
	vec4(0.2, 0.7, 0.1, 1),
	vec4(0.5, 0.7, 0.1, 1),
	vec4(0.5, 1.3, 0.1, 1),
	vec4(0.7, 1.3, 0.1, 1),
	vec4(1, 1.7, 0.1, 1)
};

// Indexes for tall blades of grass
int tallIndexes[] = {
	0, 1, 2,
	1, 3, 2,
	2, 3, 4,
	3, 5, 4,
	4, 5, 6 };

// Points for small blades of grass
vec4 smallPoints[] = {
	vec4(-0.35, 0.65, 0, 1),
	vec4(-0.1, 0.4, 0, 1),
	vec4(-0.15, 0.0, 0, 1),
	vec4(0.15, 0.0, 0, 1),
	vec4(0.15, 0.65, 0, 1),
	vec4(0.4, 0.6, 0, 1),
	vec4(0.65, 1.0, 0, 1)
};

// Indexes for small blades of grass
int smallIndexes[] = {
	4, 5, 6,
	4, 3, 5,
	2, 3, 4,
	2, 3, 1,
	0, 1, 2 };

// Returns a rotation matrix
mat4 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}


// Generates a tall blade of grass
void tallGrass(vec4 pos, float width)
{
	// Stores the world position
	float x = worldPos[0].x;
	float z = worldPos[0].z;

	// Make sure that neither x or z are 0 (prevents straight lines of identical grass being generated along an axis)
	if (x == 0.0f) x += 0.1f;
	if (z == 0.0f) z += 0.1f;

	// Get the rotation matrix for this point
	mat4 rot = rotationMatrix(vec3(0, 1, 0), 1.0f - sin(x*z));

	// Calculate the translation of the blade
	vec3 bladeTranslation = vec3(0);
	bladeTranslation = vec3(1, 0, 1) * sin(totalTime + ((pos.x + pos.z) * (x*z)) / 100.0) * windFactor * (normalize(pos).y);

	// Get the distance from the camera
	vec3 distVec = worldPos[0] - camPos;
	float dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));

	// Generate the blade
	for (int i = 0; i < tallIndexes.length(); i++){
		vec3 vTranslation = bladeTranslation * (tallPoints[tallIndexes[i]].y / 2.0f);
		vec4 position = vec4((worldPos[0] + pos.xyz + (tallPoints[tallIndexes[i]] * rot).xyz * width) + vTranslation, 1);
		gl_Position = MVP * position;
		tex_coord = tallPoints[tallIndexes[i]].xy * vec2(1, 0.5f) + vec2(sin(x*z), 0);

		// Calculate normal
		wPosition = position.xyz;
		EmitVertex();
	}
	EndPrimitive();
}

// Generates a short blade of grass
void shortGrass(vec4 pos, float width)
{
	// Stores the world position
	float x = worldPos[0].x;
	float z = worldPos[0].z;

	// Make sure that neither x or z are 0 (prevents straight lines of identical grass being generated along an axis)
	if (x == 0.0f) x += 0.1f;
	if (z == 0.0f) z += 0.1f;

	// Get the rotation matrix for this point
	mat4 rot = rotationMatrix(vec3(1, 1, 0), sin(x*cos(z)));

	// Calculate the translation of the blade
	vec3 bladeTranslation = vec3(0);
	bladeTranslation = vec3(1, 0, 1) * sin(totalTime + ((pos.x + pos.z) * (x*z)) / 100.0) * windFactor * (normalize(pos).y);

	// Get the distance from the camera
	vec3 distVec = worldPos[0] - camPos;
	float dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));

	// Generate the blade
	for (int i = 0; i < smallIndexes.length(); i++){
		vec3 vTranslation = bladeTranslation * (smallPoints[smallIndexes[i]].y / 2.0f);
		vec4 position = vec4((worldPos[0] + pos.xyz + (smallPoints[smallIndexes[i]] * rot).xyz * width) + vTranslation, 1);
		gl_Position = MVP * position;
		tex_coord = smallPoints[smallIndexes[i]].xy * vec2(1, 0.5f) + vec2(sin(x*z), 0);

		// Calculate normal
		wPosition = position.xyz;
		EmitVertex();
	}
	EndPrimitive();

}


void main() {

	// Read in the pixel related to the current point
	vec4 rgb = texture(heightTex, vec2((worldPos[0].x + count / 2) / count, (worldPos[0].z + count / 2) / count));

	// Calculate whether to cull
	float directionalCull = dot(normalize(vec3(-1, eyeDir.y > 0 ? -1 : 1, -1)*(worldPos[0] - camPos)), eyeDir);

	// Check if the height is too low, if the size is too small or if to cull
	if (rgb.r != 0.0f && rgb.b <= 0.90f && directionalCull >= 0.1f)
	{
		// Convert texture values to world values
		float s = (1.0f - texture(heightTex, vec2((worldPos[0].x + count / 2) / count
			, (worldPos[0].z + count / 2) / count)).b) * 0.65f;
		float h = rgb.r - 0.50f;
		h *= 500.0f;
		h -= -77.0f;

		// Get the distance from the camera
		float dist = distance(worldPos[0], camPos);

		// Check if the quality by checking the distance
		int i;
		int quality = dist > 450.0f ? 1 : 2;
		if (quality == 2){ // :( i can't have dynamic length loops
			for (i = 0; i < 2; i++)
			{
				// Generates a tall blade of grass
				tallGrass(vec4(1, h, pow(sin(worldPos[0].x)*i, 2), 1), s*(2.0f*(sin(worldPos[0].x + worldPos[0].z) - 0.5f) + (3.0f)*((5 - i) - 0.1f)));
			}
			for (i = 0; i < 1; i++)
			{
				// Generates a short blade of grass
				shortGrass(vec4(pow(sin(worldPos[0].z)*i, 2), h, i, 1), s*(2.0f*(sin(worldPos[0].x + worldPos[0].z) - 0.5f) + (1.5f)*((5 - i) - 0.1f)));
			}
		}
		else{
			for (i = 0; i < 1; i++)
			{
				// Generates a tall blade of grass
				tallGrass(vec4(1, h, pow(sin(worldPos[0].x)*i, 2), 1), s*(2.0f*(sin(worldPos[0].x + worldPos[0].z) - 0.5f) + (3.0f)*((5 - i) - 0.1f)));
			}
			for (i = 0; i < 1; i++)
			{
				// Generates a short blade of grass
				shortGrass(vec4(pow(sin(worldPos[0].z)*i, 2), h, i, 1), s*(2.0f*(sin(worldPos[0].x + worldPos[0].z) - 0.5f) + (3.0f)*((5 - i) - 0.1f)));
			}

		}
	}
	else{
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		EndPrimitive();
	}

}