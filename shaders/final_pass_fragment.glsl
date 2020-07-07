 #version 330

uniform int wWidth, wHeight;
const int samplesX = 2;

vec2 texCoord;
vec3 world_pos;
vec3 world_normal;

uniform vec3 eye_position;


uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
//light accumulation pass
uniform sampler2D lightTexDiffuse, lightTexSpecular;
uniform sampler2D charactersTex;

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D posTex;


uniform sampler2D hatchTex1, hatchTex2, hatchTex3;//, hatch4;


vec3 luma=vec3(0.299, 0.587, 0.114);

float changeInterval(float x, float a, float b, float c, float d)
{
	return c + (x - a)/(b - a) * (d - c);
}

float GetHatchedLight(float val)
{
	vec2 tc = texCoord * vec2(16,9) *2;
	if (val < 0.25){
		float t1 =  texture2D(hatchTex3, tc).x;
		float t2 =  texture2D(hatchTex2, tc).x;
		val = changeInterval(val, 0,0.25, 0, 1);
		return mix(t1,t2,val);
	}

	else if(val < 0.5){
		float t1 =  texture2D(hatchTex2, tc).x;
		float t2 =  texture2D(hatchTex1, tc).x;
		val = changeInterval(val, 0.25,.5, 0, 1);
		return mix(t1,t2,val);
	}
	else{// if(val < 0.75){
		float t1 =  texture2D(hatchTex1, tc).x;
		float t2 = 1;//texture2D(hatchTex2, texCoord).x;
		val = changeInterval(val, 0.5,1, 0, 1);
		return mix(t1,t2,val);
	}
	// else 
		// return 1;

}

vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}

vec3 ColorCorrect(vec3 color){
	return color;
}

uniform int solid_color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
#define outCol gl_FragData[0]
void main()
{
// asfa
	texCoord = CalcTexCoord(0,0);


	vec3 diffuseLight = texture2D(lightTexDiffuse, texCoord).xyz;
	vec3 specularLight = texture2D(lightTexSpecular, texCoord).xyz;;
	float emissiveFact = texture2D(charactersTex, texCoord).z;
	diffuseLight += vec3(emissiveFact, emissiveFact, emissiveFact);
	
	vec3 materialDiffuseColor = texture2D(diffuseTex, texCoord).xyz;
	vec3 materialSpecularColor = texture2D(specularTex, texCoord).xyz;


	// outCol = vec4(diffuseLight,1);
	// return;

	float fragDepth = texture2D(depthTex, texCoord).x;
	float frdptmd = clamp(pow(fragDepth,20),0,0.5);


   	vec3 color = materialDiffuseColor * diffuseLight + materialSpecularColor * specularLight;//apply Diffuse tex
//    vec3 color = (vec3(0.1,0.1,0.1)+ light);//apply Diffuse tex
	// if (materialDiffuseColor.r > 0.5 && materialDiffuseColor.g > 0.5 && materialDiffuseColor.b > 0.5)
		// color = vec3(1,1,1);
 	 	
		float r = rand(0.5 * vec2(gl_FragCoord.x, frdptmd) + 0.5 * vec2(frdptmd, gl_FragCoord.y));
		float rr = clamp(r, 0.0,0.5);
		// rr = pow(r, 0.25);
#define NUMLEVELS 16
	if(texture2D(charactersTex, texCoord).x == 0){
		vec3 origColor = color;
		color = 0.5 * color + 0.5 * 
				   vec3(floor(color.x * NUMLEVELS) / NUMLEVELS,
						floor(color.y * NUMLEVELS) / NUMLEVELS,
						floor(color.z * NUMLEVELS) / NUMLEVELS);
		color = rr * color + (1 - rr) * origColor;
		// color = frdptmd * rr * color + (1 - frdptmd) * color;  
	}
	

	
	outCol = vec4(ColorCorrect(color), 1);
	// return;
	float depth = texture2D(depthTex, texCoord).x;
	// depth = (depth + 1) * 0.5;
	float depth32 = pow(depth, 32);
	float depth64 = depth32 * depth32;
	float farSpace = depth64;
	float middleSpace = depth32 - depth64;
	vec4 _red  = vec4(0.5, 0.0, 0.1, 1.0);
	vec4 _gold = vec4(0.3, 0.1, 0.1, 1.0);
	vec4 _blue = vec4(0.1, 0.2, 0.5, 1.0);

	vec4 origFragData = outCol;
	outCol += 0.5 * (farSpace * _gold + middleSpace * _blue);
	outCol *= (1 - pow(farSpace, 4) + emissiveFact);

}
