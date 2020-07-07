#version 330

varying vec3 world_pos;
varying vec3 world_normal;

uniform vec3 uColor;
uniform vec3 lightPos;
uniform vec3 eyePos;

uniform mat4 Model;

void main()
{
	vec3 V = normalize(eyePos - world_pos);
	float alpha = abs(dot(normalize(world_normal), V));
	float dist = length(world_pos - lightPos);

	gl_FragColor = vec4(uColor, pow(alpha,3) / (dist * 0.1));// * 0.7 / dist);
}
