#version 330

in float texcoord;
in float opacity;

void main()
{
#define red vec3(1,0,0)
#define blue vec3(0,0,1) 
	float f = sin(15 * texcoord) * 0.25 + 0.75;
	float alpha =  texcoord * opacity * f;
	// alpha = pow(alpha, 0.5);
	gl_FragColor = vec4(0.05,0.05,0.05, pow(alpha,0.2));
	// gl_FragColor = vec4(1);	
}