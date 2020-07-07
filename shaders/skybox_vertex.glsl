#version 330

in vec3 in_position;
in vec3 in_normal;
in vec2 TexCoord;

varying vec2 texcoords;
uniform mat4 PVM;

void main()
{
	texcoords = TexCoord;
	gl_Position = PVM * vec4(in_position,1);
	gl_Position = vec4(gl_Position.x, gl_Position.y, gl_Position.w  * 0.99999, gl_Position.w);
}