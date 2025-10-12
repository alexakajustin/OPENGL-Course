#version 330											     
in vec3 pos;											     
uniform mat4 model;										     
uniform mat4 projection;								     
out vec4 vertex_color;						                 
void main()								                     
{										   				     
	gl_Position = projection * model * vec4(pos, 1.0);		 
	vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);		 
};	