#version 330

varying vec2 texcoord;

uniform sampler2D diffuseMap;

void main()
{
	gl_FragColor = texture2D(diffuseMap, texcoord);
	// gl_FragColor = vec4(1,0,0,1);		
}