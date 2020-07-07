#version 330


in vec3 in_position;

uniform mat4 Model;
uniform mat4 PV;

void main()
{	
	gl_Position = PV * Model * vec4(in_position, 1);
}
