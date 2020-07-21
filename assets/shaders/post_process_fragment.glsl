#version 330

uniform int wWidth, wHeight;

vec2 texCoords;
vec3 color;
vec3 edges;
vec3 postProcess;

uniform sampler2D frameTex;
uniform sampler2D edgeTex;
uniform sampler2D postProcessTex;
uniform sampler2D bloomTex;
uniform sampler2D ssrTex;
uniform samplerCube cubeShadowMap;

uniform sampler2D shadowTest;


vec2 CalcTexCoord(float offsetx, float offsety)
{
    //return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(2732, 800);
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

//out vec3 gl_FragColor;

void main()
{
    texCoords = CalcTexCoord(0,0);
    gl_FragColor = texture2D(frameTex, texCoords);
// 

    // gl_FragColor = 0.2 * gl_FragColor + 0.8 * vec4(texture2D(shadowTest,texCoords));
    // return;
    
    vec4 bloom = texture2D(bloomTex, texCoords);
    vec4 ssrColor = texture2D(ssrTex, texCoords);



    gl_FragColor = gl_FragColor * (vec4(1) +  5*ssrColor);

    gl_FragColor = vec4(max(gl_FragColor.x, bloom.x),
                    max(gl_FragColor.y, bloom.y),
                    max(gl_FragColor.z, bloom.z),1);

    // gl_FragColor *= vec4(1) + bloom;
    gl_FragColor *= texture2D(edgeTex ,texCoords).x;
    gl_FragColor *= 0.5 + 1  - length(texCoords - vec2(0.5));

}