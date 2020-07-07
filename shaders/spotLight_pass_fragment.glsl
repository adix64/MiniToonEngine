#version 330

uniform int wWidth, wHeight;

uniform sampler1D uToonLUT;
uniform sampler1D uEnvLUT;

vec2 texCoord;
vec3 gWorldPos;
vec3 gWorldNormal;
vec3 gLightDirection;
vec3 gEyeDirection;
vec4 gLightSpacePos;

float gShadowFactor;
float gAttenuation;
float gDiffuseFactor;


uniform sampler2D diffuseTex;
uniform sampler2D worldPosTex;
uniform sampler2D worldNormalTex;
uniform sampler2D charactersTex;
//uniform sampler2D depthTex;

#define constant x
#define linear y
#define quadratic z
struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	vec3 luminance;
	vec3 attenuation;//.constant/.linear/.quadratic
	float power;
	float cutoff;
	float blurCutoff;
	float ambientIntensity;
};

uniform sampler2D shadowMap, staticShadowMap;
uniform SpotLight spotLight;
uniform mat4 lightPV;
uniform vec3 eye_position;
uniform int shadows;



vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

#define SHADOBIAS 0.002
#define SHADOCOEF 200.0

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
    return vec2(a, b) * 0.002;
}


float ComputeShadowFactor(vec3 VertToLightWS, sampler2D theMap)
{  
	vec3 ProjCoords;
    vec2 UVCoords, UVCoords1;
	float moments1;
	float compare;
	float ret1;

	gLightSpacePos = lightPV * vec4(gWorldPos, 1);
	ProjCoords = gLightSpacePos.xyz / gLightSpacePos.w;

	compare = 0.5 * (ProjCoords.z + 1.0) + SHADOBIAS;

	UVCoords.x = 0.5 * (ProjCoords.x + 1.0);
	UVCoords.y = 0.5 * (ProjCoords.y + 1.0);

	UVCoords1 = UVCoords;// + noiseFun1(gl_FragCoord.xy);
	

	moments1 = texture2D(theMap, UVCoords1).x;
	
	
	if (compare <= moments1)
		ret1 = 1.0;
	else{
		// ret1 = 0;
   		ret1 = pow(clamp( exp( -SHADOCOEF * (compare - moments1)), 0.0, 1.0 ),1);
	}

	return ret1;
}

float changeInterval(float x, float a, float b, float c, float d)
{
	return c + (x - a)/(b - a) * (d - c);
}

float SoftShadingDiffuse(float val)
{
	return texture(uToonLUT, val).x;
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
	
	vec3 diffuseTerm = light * spotLight.color *
					spotLight.power * gAttenuation;

	return diffuseTerm;
}


vec3 ComputeSpecular()
{
	if (gShadowFactor < 0.2)
		return vec3(0);
	vec3 H = normalize(gLightDirection + gEyeDirection);
	float specPower = pow(1 + texture2D(charactersTex, texCoord).y, 10);// * 0.5;
	float specluar_factor = pow(clamp(dot(H, gWorldNormal),0,1), specPower);// uv_maps.cosinePower        
	float light = specluar_factor * spotLight.power; //careful with the specular!!!
	
    light *= gShadowFactor;

	vec3 specularTerm = light * spotLight.color * gAttenuation;

	return specularTerm;
}

void main()
{
	texCoord = gl_FragCoord.xy / vec2(wWidth, wHeight);

	gWorldPos = texture2D(worldPosTex, texCoord).xyz;
	gWorldNormal = texture2D(worldNormalTex, texCoord).xyz;

 	gEyeDirection = normalize(eye_position - gWorldPos);
    gLightDirection = gWorldPos - spotLight.position;

    float lightDirDotLightFrag = dot(normalize(gLightDirection), spotLight.direction);
    if(lightDirDotLightFrag < spotLight.cutoff)
    	discard;
    float cutoffFact = 1.0;
    if(lightDirDotLightFrag < spotLight.blurCutoff)
    	cutoffFact = changeInterval(lightDirDotLightFrag, spotLight.cutoff, spotLight.blurCutoff, 0, 1);



    float distance = length(gLightDirection);
	gAttenuation =  spotLight.attenuation.constant +                                                 
                    spotLight.attenuation.linear * distance +                                        
                    spotLight.attenuation.quadratic * distance * distance;
    gAttenuation = 1 / gAttenuation;

   	gShadowFactor = 1;

   	if (shadows == 1)
   		 gShadowFactor = ComputeShadowFactor(gLightDirection, shadowMap) *  
   						 ComputeShadowFactor(gLightDirection, staticShadowMap);

    gLightDirection = normalize(-gLightDirection);
    
	gl_FragData[0] = vec4(ComputeDiffuse() * cutoffFact, 1);
	gl_FragData[1] = vec4(ComputeSpecular() * cutoffFact, 1);

}
