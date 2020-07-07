#version 330
uniform int wWidth, wHeight;
uniform sampler2D frameTex;

vec3 luma = vec3(0.299, 0.587, 0.114);

vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

void main()
{
    vec2 texCoords = CalcTexCoord(0,0);

	gl_FragColor.x = dot(texture2D(frameTex, texCoords).xyz, luma); 
}