#version 440

uniform mat4 MVP;

uniform float totalTime = 0.0f;
uniform float lifeTime = 1.0f;
uniform float xCoordInterval = 1.0f;
uniform float yCoordInterval = 1.0f;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Incoming
in vec3 worldPos[];

// Outgoing
layout(location = 0) out vec2 tex_coord;
layout(location = 2) out vec3 normal;

// Points for small blades of grass
vec4 quadPoints[] = {
	vec4(-1.0, -1.0, 0.0, 1.0),
	vec4(1.0, -1.0, 0.0, 1.0),
	vec4(-1.0, 1.0, 0.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0)
};
vec2 quadCoords[] = {
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 1.0)
};

float particle_size = 1.0f;
void main() {

	// Generate the blade
	for (int i = 0; i < quadPoints.length(); i++){
		vec4 position = gl_in[0].gl_Position + quadPoints[i] * particle_size;
		gl_Position = position;
		float xCount = 1.0f / xCoordInterval;
		float yCount = 1.0f / yCoordInterval;
		float currentImage = (lifeTime - totalTime) / lifeTime;
		currentImage *= (xCount * yCount);

		float xCoord = xCoordInterval * currentImage;
		xCoord = mod(xCoord, 1.0f);
		xCoord -= mod(xCoord, xCoordInterval);
		xCoord -= xCoordInterval * 2;
		float yCoord = yCoordInterval * currentImage;
		yCoord = (yCoord - mod(yCoord, 1.0f)) * yCoordInterval;

		tex_coord = vec2(quadCoords[i].x == 0.0 ? xCoord : xCoord + xCoordInterval, quadCoords[i].y == 0.0 ? yCoord : yCoord + yCoordInterval);
		EmitVertex();
	}
	EndPrimitive();

}