#version 330

in vec2 texcoords;

uniform sampler2D diffuseMap;

void main()
{
	gl_FragColor = texture2D(diffuseMap, texcoords);
	// gl_FragColor = vec4(1,0,0,1);		
}