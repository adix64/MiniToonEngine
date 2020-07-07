#version 330

in vec3 in_position;

uniform mat4 PV;
uniform mat4 Model;

void main()
{	
	vec3 world_pos = (Model * vec4(in_position, 1)).xyz;
	gl_Position = PV * vec4(world_pos,1);
}
