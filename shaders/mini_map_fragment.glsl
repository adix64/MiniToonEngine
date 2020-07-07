#version 330

varying float vLinDepth;

void main()
{
	float alpha = 0.75;
	if(length(gl_FragCoord.xy - vec2(256,256)) > 256)
		alpha = 0;
	float outF = 1 - vLinDepth;
	gl_FragColor = vec4(outF,outF, outF, alpha);
}