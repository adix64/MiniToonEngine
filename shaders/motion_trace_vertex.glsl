#version 330

in vec3 vPosition;
in float vTexCoord;
in float vOpacity;

out float opacity;
out float texcoord;

uniform mat4 PV;

void main()
{		 
	opacity = vOpacity;
	texcoord = vTexCoord;
	gl_Position = PV * vec4(vPosition,1);
}
