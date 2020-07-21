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
    
    // vec2 left = texture2D(cubeShadowFace, CalcTexCoord(-1.5,0)).xy;
    // vec2 middle = texture2D(cubeShadowFace, texCoords).xy;
    // vec2 right = texture2D(cubeShadowFace, CalcTexCoord(1.5,0)).xy;

    float left = texture2D(cubeShadowFace, CalcTexCoord(-1.5,0)).x;
    float middle = texture2D(cubeShadowFace, texCoords).x;
    float right = texture2D(cubeShadowFace, CalcTexCoord(1.5,0)).x;

    gl_FragColor.x = (clamp(left,0,1) + clamp(right, 0,1) + clamp(middle,0,1)) * 0.33333;
    // gl_FragColor.y = (clamp(left.y,0,1) + clamp(right.y,0,1) + clamp(middle.y,0,1)) * 0.33333;
}