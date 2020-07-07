#version 330

uniform int wWidth, wHeight;
uniform sampler2D cubeShadowFace;
vec2 texCoords;
vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

void main()
{
    texCoords = CalcTexCoord(0,0);  
    
    float up = texture2D(cubeShadowFace, CalcTexCoord(0,1.5)).x;
    float middle = texture2D(cubeShadowFace, texCoords).x;
    float down = texture2D(cubeShadowFace, CalcTexCoord(0,-1.5)).x;

    gl_FragColor.x = (clamp(up,0,1) + clamp(down,0,1) + clamp(middle,0,1)) * 0.33333;
    // gl_FragColor.y = (clamp(up.y,0,1) + clamp(down.y,0,1) + clamp(middle.y,0,1)) * 0.33333;
}