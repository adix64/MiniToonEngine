#version 330

uniform int wWidth, wHeight;
uniform sampler2D frameTex;
vec2 texCoords;
vec2 CalcTexCoord(float offsetx, float offsety)
{
  return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

void main()
{ 
    texCoords = CalcTexCoord(0,0);  
    vec3 luma = vec3(0.299, 0.587, 0.114);
    vec3 left = texture2D(frameTex, CalcTexCoord(-1.5,0)).xyz;
    vec3 middle = texture2D(frameTex, texCoords).xyz;
    vec3 right = texture2D(frameTex, CalcTexCoord(1.5,0)).xyz;

    if(dot(luma, left) < 0.5)
    	left = vec3(0,0,0);
    if(dot(luma, middle) < 0.5)
    	middle = vec3(0,0,0);
    if(dot(luma, right) < 0.5)
    	right = vec3(0,0,0);

    gl_FragColor =vec4((left * 0.25 + middle * 0.5 + right * 0.25), 1);
}