#version 330

//@@@@@ IN @@@@@
in vec3 in_position;
in vec3 in_normal;

//@@@@@ OUT @@@@@
varying vec3 world_pos;
varying vec3 world_normal;

uniform mat4 PV;
uniform mat4 Model;

void main()
{		 
	world_pos = (Model * vec4(in_position, 1)).xyz;
	world_normal = normalize(vec3(Model * vec4(in_normal,0)));
	gl_Position = PV * vec4(world_pos,1);
}
