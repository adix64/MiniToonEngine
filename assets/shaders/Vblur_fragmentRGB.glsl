#version 330

uniform int wWidth, wHeight;
uniform sampler2D texture;
vec2 texCoords;
vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

void main()
{
    texCoords = CalcTexCoord(0,0);  
    
    vec3 up = texture2D(texture, CalcTexCoord(0,1.5)).xyz;
    vec3 middle = texture2D(texture, texCoords).xyz;
    vec3 down = texture2D(texture, CalcTexCoord(0,-1.5)).xyz;

    gl_FragColor = vec4((up + down + middle) * 0.33333, 1);
}