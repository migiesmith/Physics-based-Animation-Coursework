#version 410

// Sampler used to get texture colour
uniform sampler2D tex;

// Sampler used to apply a normal map to the pixel
uniform sampler2D normalTex;
uniform bool hasNormalMap;

// Sampler used to get the depth from the light's depth texture
uniform sampler2D shadowMap;

// Material values
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Material from mesh
uniform material mat;

uniform vec3 ambientLightDir;

uniform vec3 eyeDir;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
layout(location = 2) in vec3 norm;
// Outgoing colour
layout (location = 0) out vec4 out_colour;


void main()
{
	// Normalize the normal
	vec3 n = normalize(norm);
	
    // Calcualate the half vector
    vec3 hVec = normalize(ambientLightDir + eyeDir);  
	
	// Calcualate the diffuse value
	vec4 diffuse = (mat.diffuse_reflection * texture(tex, tex_coord)) * max(dot(n, ambientLightDir), 0.0);

	float shine = mat.shininess;
	
	// Check if the mesh has a bump map
	if(hasNormalMap){
		vec4 normMapColour = texture(normalTex, tex_coord);
		vec3 normMap = normMapColour.rbg * 2.0 -1.0;
		n += normMap;
	}
	
	n = normalize(n);
	
	// Calcualate the specular value
	vec4 specular = (mat.specular_reflection * texture(tex, tex_coord)) * pow(max(dot(n, hVec), 0.0), shine);

	
	// Merge the light values together to create the colour for the pixel
	out_colour = max(texture(tex, tex_coord) * diffuse + (specular) * mat.emissive,
					 texture(tex, tex_coord) * diffuse * mat.emissive);

	out_colour += vec4(0.1,0.1,0.1,0);
	out_colour *= vec4(1.1,1.1,1.1,1);
	out_colour.a = texture(tex, tex_coord).a;

}