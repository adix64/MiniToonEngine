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
    
    vec3 left = texture2D(texture, CalcTexCoord(-1.5, 0)).xyz;
    vec3 middle = texture2D(texture, texCoords).xyz;
    vec3 right = texture2D(texture, CalcTexCoord(1.5, 0)).xyz;

    gl_FragColor = vec4((left + middle + right) * 0.33333, 1);
}