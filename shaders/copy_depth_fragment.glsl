#version 330

uniform int wWidth, wHeight;

uniform sampler2D depthTex;

void main()
{
	vec2 texCoord = gl_FragCoord.xy / vec2(wWidth, wHeight);
	gl_FragDepth = texture2D(depthTex, texCoord).x;
}
