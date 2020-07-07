#version 330

//@@@@@ IN @@@@@
in vec3 in_position;
in vec3 in_normal;
in vec2 TexCoord;

//@@@@@ OUT @@@@@
varying vec3 world_pos;
varying vec3 world_normal;
varying vec2 texcoord;

//uniform mat4 skinning_matrices[100];
uniform mat4 PV;
uniform mat4 Model;

void main()
{	
	texcoord = TexCoord;
	 
	world_pos = (Model * vec4(in_position, 1)).xyz;
	world_normal = normalize((Model * vec4(in_normal,0)).xyz);

	gl_Position = PV * vec4(world_pos,1);
}
