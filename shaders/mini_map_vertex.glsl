#version 330

//@@@@@ IN @@@@@
in vec3 in_position;
in vec3 in_normal;
in vec2 TexCoord;

//@@@@@ OUT @@@@@
varying float vLinDepth;
vec3 world_pos, screenSpacePos;
//uniform mat4 skinning_matrices[100];
uniform mat4 P, V;
uniform mat4 Model;
void main()
{	 
	world_pos = (Model * vec4(in_position, 1)).xyz;
	screenSpacePos = vec3(V * vec4(world_pos,1));

	vLinDepth = length(screenSpacePos) / 100;

	gl_Position = P * vec4(screenSpacePos,1);
}