#version 330				

in vec4 vertex_color;	
in vec2 TexCoord;
in vec3 Normal;

out vec4 colour;			

struct DirectionalLight {
	vec3 ambientColour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main()								         
{									
	vec4 ambientColour = vec4(directionalLight.ambientColour, 1.0f) * directionalLight.ambientIntensity;

	// float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);

	float diffuseFactor = max(dot(normalize(Normal), -normalize(directionalLight.direction)), 0.0f);
	
	vec4 diffuseColor = vec4(directionalLight.ambientColour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	colour = texture(theTexture, TexCoord) * (ambientColour + diffuseColor);          
};	