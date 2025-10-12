#version 330							         
out vec4 colour;								
in vec4 vertex_color;							 
uniform vec4 uniform_colour;				    
void main()								         
{										   	     
	colour = vertex_color;      		         
};	