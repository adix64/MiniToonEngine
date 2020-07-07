#version 330
// #define MYMETHOD_MINSHADOWFACTOR
struct DirectionalLight {
	vec3 direction;
	vec3 color;
	vec3 luminance;
	float power;
	float ambientIntensity;
};

uniform sampler1D uToonLUT;
uniform sampler1D uEnvLUT;

uniform sampler2D shadowMap0, shadowMap1, shadowMap2, shadowMap3;
uniform mat4 lightPV[4];
uniform DirectionalLight uLight;
uniform vec3 eye_position;
uniform int shadows;

uniform int wWidth, wHeight;
uniform sampler2D diffuseTex;
uniform sampler2D worldPosTex;
uniform sampler2D worldNormalTex;
uniform sampler2D charactersTex;
uniform sampler2D depthTex;
uniform mat4 PV;

vec2 texCoord;
vec3 gWorldPos;
vec3 gWorldNormal;
vec3 gTexCoordinates;
vec3 gEyeDirection;
vec4 gLightSpacePos;
float gFragDepth;
float gShadowFactor;
float gDiffuseFactor;

int minCID;

////////////////////////////////////////////////////////////////////////////////////////////////////

float changeInterval(float x, float a, float b, float c, float d)
{
	return c + (x - a)/(b - a) * (d - c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

vec2 noiseFun1(vec2 co){
	float s = sin(dot(co.xy ,vec2(12.9898,78.233)));
    float a = fract(s * 43758.5453) - 0.5;
    float b = fract(s * 2315.1511) - 0.5;
    return vec2(a, b) * 0.002;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

vec2 noiseFun2(vec2 co){
	float s = cos(dot(co.xy ,vec2(13.9898,81.233)));
    float a = fract(s * 41358.5453) - 0.5;
    float b = fract(s * 2031.1511) - 0.5;
    return vec2(a, b) * 0.002;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ComputeShadowFactor()
{
#define SHADOBIAS 0.001 //peter panning vs self shadowing
#define SHADOCOEF 240.0 // shadow coeffecient - change this to to affect shadow darkness/fade
// #define BLEND_BETWEEN_CASCADES
	vec3 ProjCoords;
    vec2 UVCoords, UVCoords1, UVCoords2;
	float moments1, moments2;
	float compare;
	float ret;
	minCID = 0;
	float cidBlendFact = -1.0;
	bool isCrazy = false;



	for (int cid = 0; cid < 4; cid++)
	{
		gLightSpacePos = lightPV[cid] * vec4(gWorldPos, 1);
		ProjCoords = gLightSpacePos.xyz / gLightSpacePos.w;
		UVCoords.x = 0.5 * (ProjCoords.x + 1.0);
		UVCoords.y = 0.5 * (ProjCoords.y + 1.0);
		if( UVCoords.x >= 0.0 && UVCoords.x <= 1.0 &&
			UVCoords.y >= 0.0 && UVCoords.y <= 1.0 )
		{
#ifdef BLEND_BETWEEN_CASCADES		
			if(UVCoords.x <= 0.1)
			{
				cidBlendFact = UVCoords.x / 0.1;
			}else if(UVCoords.x >= 0.9)
			{
				cidBlendFact = (1 - UVCoords.x) / 0.1;
			}
			if(UVCoords.y <= 0.1)
			{
				cidBlendFact = max(0, cidBlendFact) + UVCoords.y / 0.1;
			}else if(UVCoords.y >= 0.9)
			{
				cidBlendFact = max(0, cidBlendFact) + (1 - UVCoords.y) / 0.1;
			}
#endif
			minCID = cid;
			break;
		}
	} 
	// if(isCrazy)
		// crazy = 4;
	compare = 0.5 * (ProjCoords.z + 1.0) + SHADOBIAS;
	
	UVCoords1 = UVCoords;// + noiseFun1(gl_FragCoord.xy);
	

	if(minCID == 0){
		moments1 = texture2D(shadowMap0, UVCoords1).x;
	}else if(minCID == 1){
		moments1 = texture2D(shadowMap1, UVCoords1).x;
	}else if(minCID == 2){
		moments1 = texture2D(shadowMap2, UVCoords1).x;
	}else {
		moments1 = texture2D(shadowMap3, UVCoords1).x;
	}


	if (compare <= moments1)
		ret = 1.0;
	else
	{
		float compareMinusMoments = (compare - moments1);
   		ret = exp( -SHADOCOEF * compareMinusMoments);/// pow((1 +compareMinusMoments ),8));
	}
	
	
#ifdef BLEND_BETWEEN_CASCADES
	if(cidBlendFact > 0 && minCID < 3)
	{
		int idx = (minCID + 1) % 4;

		gLightSpacePos = lightPV[idx] * vec4(gWorldPos, 1);
		ProjCoords = gLightSpacePos.xyz / gLightSpacePos.w;

		compare = 0.5 * (ProjCoords.z + 1.0) + SHADOBIAS;

		UVCoords.x = 0.5 * (ProjCoords.x + 1.0);
		UVCoords.y = 0.5 * (ProjCoords.y + 1.0);
		
		UVCoords1 = UVCoords + noiseFun1(gl_FragCoord.xy);
		UVCoords2 = UVCoords + noiseFun2(gl_FragCoord.xy);
		
		if(idx == 0){
			moments1 = texture2D(shadowMap0, UVCoords1).x;
			moments2 = texture2D(shadowMap0, UVCoords2).x;
		}else if(idx == 1){
			moments1 = texture2D(shadowMap1, UVCoords1).x;
			moments2 = texture2D(shadowMap1, UVCoords2).x;
		}else if(idx == 2){
			moments1 = texture2D(shadowMap2, UVCoords1).x;
			moments2 = texture2D(shadowMap2, UVCoords2).x;
		}else {
			moments1 = texture2D(shadowMap3, UVCoords1).x;
			moments2 = texture2D(shadowMap3, UVCoords2).x;
		}


		float ret2;

		if (compare <= moments1)
			ret2 = 1.0;
		else
		{
			float compareMinusMoments = (compare - moments1);
	   		ret2 = exp( -SHADOCOEF * compareMinusMoments);/// pow((1 +compareMinusMoments ),8));
		}

		if (compare <= moments2)
			ret2 = (ret2 + 1.0) * 0.5;
		else
		{
			float compareMinusMoments = (compare - moments2);
	   		ret2 += exp( -SHADOCOEF * compareMinusMoments);/// pow((1 +compareMinusMoments ),8));
			ret2 = ret2 * 0.5;
		}

		ret = ret * cidBlendFact + (1.0 - cidBlendFact) * ret2;
		// moments = texture2D(shadowMap[idx], UVCoords).x;
		// if (compare <= moments)
		// 	ret = ret * cidBlendFact + (1.0 - cidBlendFact);
		// else
		// {
		// 	float ret2 = exp( -SHADOCOEF * (compare - moments));
	 //   		ret = ret * cidBlendFact + 
	 //   			(1.0 - cidBlendFact) * ret2;
		// }
	}
#endif
	return clamp(ret, 0, 1);// * ret;
	// return pow(ret,4);

}



vec3 ComputeDiffuse()
{
    gDiffuseFactor = dot(gWorldNormal, uLight.direction);
    gDiffuseFactor = clamp(gDiffuseFactor,0,1);

    float light = gDiffuseFactor;

	light *= gShadowFactor;

	if(texture2D(charactersTex, texCoord).x != 0)
	{
		light = texture(uToonLUT, clamp(light,0,1)).x;
	}//else light = texture(uEnvLUT,clamp(light,0,1)).x;
	
	vec3 diffuseTerm = light * uLight.color *
					uLight.power;

	return diffuseTerm;
}


vec3 ComputeSpecular()
{
	if (gShadowFactor < 0.2)
		return vec3(0);
	vec3 H = normalize(uLight.direction + gEyeDirection);
	float specPower = pow(1 + texture2D(charactersTex, texCoord).y, 10);// * 0.5;
	float specluar_factor = pow(clamp(dot(H, gWorldNormal),0,1), specPower);// uv_maps.cosinePower        
	float light = specluar_factor * uLight.power; //careful with the specular!!!
	
    light *= gShadowFactor;

	vec3 specularTerm = light * uLight.color;

	return specularTerm;
}


void main()
{
	texCoord = gl_FragCoord.xy / vec2(wWidth, wHeight);

	gWorldPos = texture2D(worldPosTex, texCoord).xyz;
	vec4 viewSpacePos = PV * vec4(gWorldPos, 1);
	gFragDepth = viewSpacePos.z /viewSpacePos.w;

	gWorldNormal = texture2D(worldNormalTex, texCoord).xyz;

	// gFragDepth = texture2D(depthTex, texCoord).x;

 	gEyeDirection = normalize(eye_position - gWorldPos);
 
    float distance = length(uLight.direction);

   	gShadowFactor = 1;

   	if (shadows == 1)
   	{
   		gShadowFactor = ComputeShadowFactor();
	}
	gl_FragData[0] = vec4(ComputeDiffuse(), 1);
	gl_FragData[1] = vec4(ComputeSpecular(), 1);
}
// -1.0_______0.8_______0.91_______0.98_______1.0