#version 440

// Spot light data
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};

// Material data
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eyePos;
// Sampler used to get texture colour
uniform sampler2D tex;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 1) in vec3 normal;
// Incoming texture coordinate
layout (location = 2) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 out_colour;

void main()
{
	
	//calculate the light's direction
	vec3 lightDir = normalize(spot.position - position);
	
	// calculate the distance from the light
	float d = distance(spot.position, position);

	// calculate the attenuaution value
	float attenuation = spot.constant + spot.linear * d + spot.quadratic * d * d;

	// calculate the intensity value
	float intensity = pow(max(dot(lightDir, -spot.direction), 0.0), spot.power);
	
	// calculate the light colour
	vec4 lightColour = (intensity / attenuation) * spot.light_colour * texture(tex, tex_coord);

	// calculate the view's direction
	vec3 viewDir = normalize(eyePos - position);

	
	/*
	 calculate the diffuse and specular then merge the light values together to create the colour for the pixel
	*/
	// calculate the diffuse value
	vec4 diffuse = (mat.diffuse_reflection * lightColour) * max(dot(normal, lightDir), 0.0);
	vec3 hVec = normalize(lightDir + viewDir);
	vec4 specular = (mat.specular_reflection * lightColour) * pow(max(dot(normal, hVec), 0.0), mat.shininess);
	vec4 tex_colour = texture(tex, tex_coord);
	out_colour = ((mat.emissive + diffuse) * tex_colour) + specular;
}