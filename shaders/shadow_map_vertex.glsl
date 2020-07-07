#version 330

//@@@@@ IN @@@@@
in vec3 in_position;
in vec3 in_normal;
in vec2 TexCoord;

//@@@@@ OUT @@@@@
varying vec4 screenSpacePos;
vec3 world_pos;
//uniform mat4 skinning_matrices[100];
uniform mat4 PV;
uniform mat4 Model;
void main()
{	 
	world_pos = (Model * vec4(in_position, 1)).xyz;

	gl_Position = PV * vec4(world_pos,1);
	screenSpacePos = gl_Position;
}
