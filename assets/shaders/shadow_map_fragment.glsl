#version 330

varying vec4 screenSpacePos;

void main()
{
	float depth = gl_FragCoord.z;
	// float dx = dFdx(depth);
	// float dy = dFdy(depth);
	gl_FragColor.x = depth;
	// gl_FragColor.y = pow(depth, 2.0) + 0.25*(dx*dx + dy*dy);
}