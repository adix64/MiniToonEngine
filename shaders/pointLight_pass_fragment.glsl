#version 330
uniform int wWidth, wHeight;

uniform sampler1D uToonLUT;
uniform sampler1D uEnvLUT;

#define constant x
#define linear y
#define quadratic z

struct PointLight {
	vec3 position;
	vec3 color;
	vec3 luminance;
	vec3 attenuation;//.constant/.linear/.quadratic
	float power;
	float ambientIntensity;
};

uniform samplerCube cubeShadowMap, staticCubeShadowMap;
uniform PointLight pointLight;

uniform vec3 eye_position;
uniform int shadows;

vec2 texCoord;
vec3 gWorldPos;
vec3 gWorldNormal;
vec3 gLightDirection;
vec3 gEyeDirection;
float gShadowFactor;
float gAttenuation;
float gDiffuseFactor;


uniform sampler2D diffuseTex;
uniform sampler2D worldPosTex;
uniform sampler2D worldNormalTex;
uniform sampler2D charactersTex;
//uniform sampler2D depthTex;

vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

float VectorToDepthValue(vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 1000.0;
    const float n = 1.0;
    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}
vec2 noiseFun1(vec2 co){
	float s = sin(dot(co.xy ,vec2(12.9898,78.233)));
    float a = fract(s * 43758.5453) - 0.5;
    float b = fract(s * 2315.1511) - 0.5;
    return vec2(a, b) * 0.002;
}

vec2 noiseFun2(vec2 co){
	float s = cos(dot(co.xy ,vec2(13.9898,81.233)));
    float a = fract(s * 41358.5453) - 0.5;
    float b = fract(s * 2031.1511) - 0.5;
    return vec2(a, b) * 0.05;
}


#define SHADOBIAS 0.002
#define SHADOCOEF 300.0 // shadow coeffecient - change this to to affect shadow darkness/fade
float ComputeShadowFactor(vec3 VertToLightWS, samplerCube theCubeMap)
{  
// return 0.0;
////////////////-------------------------------------OLD STUFF--------------------------------------------------- 
	float ret;
	float compare = VectorToDepthValue(VertToLightWS) + SHADOBIAS;
	
	vec2 offset2d;
	vec3 offset; 

	offset2d = noiseFun1(gl_FragCoord.xy);
	offset = vec3(offset2d.x, offset2d.x, offset2d.y);  

    float sampleDepth = texture(theCubeMap, VertToLightWS + offset).x;

//ESM
    if (compare <= sampleDepth)
		return 1.0;
    else
    	ret = exp( -SHADOCOEF * (compare - sampleDepth));
	return clamp(ret,0,1);
}

float changeInterval(float x, float a, float b, float c, float d)
{
	return c + (x - a)/(b - a) * (d - c);
}

float SoftShadingDiffuse(float val)
{
	return texture(uToonLUT, clamp(val,0,1)).x;
}

vec3 ComputeDiffuse()
{
    gDiffuseFactor = dot(gWorldNormal, gLightDirection);
    gDiffuseFactor = clamp(gDiffuseFactor,0,1);

    float light = gDiffuseFactor;

	light *= gShadowFactor;

	if(texture2D(charactersTex, texCoord).x != 0)
	{
		light = SoftShadingDiffuse(light);
	}else light = texture(uEnvLUT,clamp(light,0,1)).x;
	
	vec3 diffuseTerm = light * pointLight.color *
					pointLight.power * gAttenuation;

	return diffuseTerm;
}


vec3 ComputeSpecular()
{
	if (gShadowFactor < 0.2)
		return vec3(0);
	vec3 H = normalize(gLightDirection + gEyeDirection);
	float specPower = pow(1 + texture2D(charactersTex, texCoord).y, 10);// * 0.5;
	float specluar_factor = pow(clamp(dot(H, gWorldNormal),0,1), specPower);// uv_maps.cosinePower        
	float light = specluar_factor * pointLight.power; //careful with the specular!!!
	
    light *= gShadowFactor;

	vec3 specularTerm = light * pointLight.color * gAttenuation;

	return specularTerm;
}
void main()
{
	texCoord = gl_FragCoord.xy / vec2(wWidth, wHeight);

	gWorldPos = texture2D(worldPosTex, texCoord).xyz;
	gWorldNormal = texture2D(worldNormalTex, texCoord).xyz;

 	gEyeDirection = normalize(eye_position - gWorldPos);
    gLightDirection = gWorldPos - pointLight.position;

    float distance = length(gLightDirection);
	gAttenuation =  pointLight.attenuation.constant +                                                 
                    pointLight.attenuation.linear * distance +                                        
                    pointLight.attenuation.quadratic * pow(distance,2);
    gAttenuation = 1 / gAttenuation;

   	gShadowFactor = 1;

   	if (shadows == 1)
   		 gShadowFactor = ComputeShadowFactor(gLightDirection, cubeShadowMap) * 
   						 ComputeShadowFactor(gLightDirection, staticCubeShadowMap);

    gLightDirection = normalize(-gLightDirection);
    
	gl_FragData[0] = vec4(ComputeDiffuse(), 1);

	gl_FragData[1] = vec4(ComputeSpecular(), 1);
}
