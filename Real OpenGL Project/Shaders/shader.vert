#version 330	

layout(location = 0) in vec3 pos;		
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
	
out vec4 vertex_color;	
out vec2 TexCoord;
out vec3 Normal;


uniform mat4 model;										     
uniform mat4 view;
uniform mat4 projection;

void main()								                     
{										   				     
	gl_Position = projection * view * model * vec4(pos, 1.0f);		 
	vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);		

	TexCoord = tex; 

	// so that normal is ok when scaling non uniformly
	Normal = mat3(transpose(inverse(model))) * norm; 
};	