#version 410

uniform mat4 MVP;

uniform float lifeTime = 1.0f;
uniform float xCoordInterval = 1.0f;
uniform float yCoordInterval = 1.0f;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Incoming
in vec3 worldPos[];
layout (location = 3) in float testTime[1];

// Outgoing
layout(location = 0) out vec2 tex_coord;
layout(location = 2) out vec3 normal;

// Points for small blades of grass
vec4 quadPoints[] = vec4[](
	vec4(-1.0, -1.0, 0.0, 1.0),
	vec4(1.0, -1.0, 0.0, 1.0),
	vec4(-1.0, 1.0, 0.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0)
);
vec2 quadCoords[] = vec2[](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 1.0)
);

float particle_size = 1.0f;

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

void main() {

	// Generate the blade
	for (int i = 0; i < quadPoints.length(); i++){
		vec4 position = gl_in[0].gl_Position + (quadPoints[i] * particle_size ) * rotationMatrix(vec3(0, 0, 1), testTime[0] * length(gl_in[0].gl_Position.xyz));
		gl_Position = position;


		float xCount = 1.0f / xCoordInterval;
		float yCount = 1.0f / yCoordInterval;
		float currentImage = (lifeTime - testTime[0]) / lifeTime;
		currentImage *= (xCount * yCount);

		float xCoord = xCoordInterval * currentImage;
		xCoord = mod(xCoord, 1.0f);
		xCoord -= mod(xCoord, xCoordInterval);
		xCoord -= xCoordInterval;
		float yCoord = yCoordInterval * currentImage;
		yCoord = (yCoord - mod(yCoord, 1.0f)) * yCoordInterval;


		tex_coord = vec2(quadCoords[i].x == 0.0 ? xCoord : xCoord + xCoordInterval, quadCoords[i].y == 0.0 ? yCoord : yCoord + yCoordInterval);
		EmitVertex();
	}
	EndPrimitive();

}