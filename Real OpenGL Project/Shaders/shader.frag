#version 330				

in vec4 vertex_color;	
in vec2 TexCoord;

out vec4 colour;			

struct DirectionalLight {
	vec3 ambientColour;
	float ambientIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main()								         
{									
	vec4 ambientColour = vec4(directionalLight.ambientColour, 1.0f) * directionalLight.ambientIntensity;
	colour = texture(theTexture, TexCoord) * ambientColour;         
};	