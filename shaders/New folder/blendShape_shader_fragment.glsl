#version 120
/*[[[[[[[[[[[[[[[[[[[[  CEL SHADING  ]]]]]]]]]]]]]]]]]]]]]]*/
const int levels = 3;
const float scaleFactor = 1.0 / levels;
/*[[[[[[[[[[[[[[[[[[[[[[[[[[[[ ]]]]]]]]]]]]]]]]]]]]]]]]]]]]*/

/*======================= LIGHT DATA ======================*/
#define MAX_NUM_LIGHTS 20
#define constant x
#define linear y
#define quadratic z

uniform mat4 View, Projection;

struct pointLight {
	vec3 position;
	vec3 color;
	vec3 attenuation;//.constant/.linear/.quadratic
	float power;
	float ambientIntensity;
};

uniform int num_lights;
uniform samplerCube shadowMap;
uniform pointLight lights[MAX_NUM_LIGHTS];
/*========================================================*/

/*-------------------- MATERIAL INFO ---------------------*/
struct Texture{
	int diffuseMapON;
	int alphaMapON;
	int bumpMapON;
	int specularMapON;
	float cosinePower;
	float specularIntensity;
};
uniform Texture uv_maps;
uniform sampler2D diffuseMap;
uniform sampler2D alphaMap;
uniform sampler2D bumpMap;
uniform sampler2D specularMap;
uniform vec3 in_color;
uniform int toon_shading;

/*--------------------------------------------------------*/


uniform vec3 eye_position;

/*in*/varying vec3 world_pos;
/*in*/varying vec3 world_normal;
/*in*/varying vec2 texcoord;

uniform int shadows;


#define EPSILON 0.05
/*............................ POINT LIGHT .................................*/
#define OFFSET 0.05
float computePlight_shadowFactor(vec3 light_direction, float distance)
{	

    float lightReach = textureCube(shadowMap, light_direction).r;
    if (distance > lightReach + EPSILON)
    	return 0.5;
    return 1.0;
}
/*float computePlight_shadowFactor(vec3 light_direction, float distance)
{	

//	light_direction = normalize(light_direction);
	int occluders = 1;
    float lightReach;
    vec3 offset_direction;

    lightReach = texture(shadowMap, light_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

    offset_direction = light_direction + vec3(-OFFSET, 0, 0);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

    offset_direction = light_direction + vec3(OFFSET, 0, 0);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

    offset_direction = light_direction + vec3(0, -OFFSET, 0);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

	offset_direction = light_direction + vec3(0, OFFSET, 0);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;
	
	offset_direction = light_direction + vec3(0, 0, -OFFSET);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

    offset_direction = light_direction + vec3(0, 0, OFFSET);
    lightReach = texture(shadowMap, offset_direction).r;
    if (distance > lightReach + EPSILON)
    	occluders++;

    return 1.0 / occluders;
}
*/
                                                                                      
vec3 computePlight(pointLight pl, vec3 eye_direction, int i)                       
{                                                                            
    vec3 light_direction = world_pos - pl.position;
    float distance = length(light_direction);
   	float shadow_factor = computePlight_shadowFactor(light_direction, distance);
    light_direction = normalize(-light_direction);
    
	float light = 0;
    float diffuse_factor = dot(world_normal, light_direction);
  

    if(toon_shading == 0){
	    if (diffuse_factor > 0.0) {                                                                
	        light += diffuse_factor;
	        vec3 light_reflect = reflect(-light_direction, world_normal);                    
	        float specluar_factor = pow(clamp(dot(eye_direction, light_reflect),0,1), 2);               /// uv_maps.cosinePower        
            light += specluar_factor; //careful with the specular!!!0
	    }/*
    	diffuse_factor = floor(diffuse_factor * (levels+3)) *(1 / (levels+3));
	    if (diffuse_factor > 0.0) {                                                                
	        light += diffuse_factor;
	        vec3 light_reflect = reflect(-light_direction, world_normal);                     
	        float specluar_factor = pow(clamp(dot(eye_direction, light_reflect), 0, 1), uv_maps.cosinePower);                                      
            float spec_mask = (specluar_factor > 0.2) ? 1 : 0;
            specluar_factor = floor(specluar_factor * (levels+3)) * (1 / (levels+3)) * spec_mask;
            light += specluar_factor; //careful with the specular!!!
	    }*/
    }else{
    	diffuse_factor = floor(diffuse_factor * levels) * scaleFactor;

	    if (diffuse_factor > 0.0) {                                                                
	        light += diffuse_factor;
	        vec3 light_reflect = reflect(-light_direction, world_normal);                     
	        float specluar_factor = pow(clamp(dot(eye_direction, light_reflect), 0, 1), uv_maps.cosinePower);                                      
            float spec_mask = (specluar_factor > 0.2) ? 1 : 0;
            specluar_factor = floor(specluar_factor * levels) * scaleFactor * spec_mask;
           // light += specluar_factor; //careful with the specular!!!
	    }
	}
	vec3 color;
	if(shadows == 1)
    	color = pl.ambientIntensity + light * pl.color * pl.power * shadow_factor;
    else
    	color = light * pl.color * pl.power;

    float attenuation =  pl.attenuation.constant +                                                 
                         pl.attenuation.linear * distance +                                        
                         pl.attenuation.quadratic * pow(distance,2);
 	attenuation =  		1+                                                 
                         0.0002 * distance +                                        
                         0.0002 * pow(distance,2);

	return color * (2/attenuation);                                                             
}                            

// Output data from fragment shader
// out vec3 gl_FragColor;

uniform int solid_color;

void main(){
	vec3 eye_direction = normalize ( eye_position - world_pos);
	if(solid_color == 0){
		vec3 color = vec3(0,0,0);	
		vec3 materialDiffuseColor;
	
	
	 	if(uv_maps.diffuseMapON == 0)
 			materialDiffuseColor = in_color;		
 		else
	 		materialDiffuseColor = texture2D(diffuseMap, texcoord).xyz;
	 	

	 	materialDiffuseColor = vec3(0.1,0.1,0.1);

	 	//materialDiffuseColor = in_color;
	 //	materialDiffuseColor = texture(diffuseMap, texcoord).xyz;
	 	

	 	if(dot(world_normal, eye_direction) < -0.02) materialDiffuseColor = vec3(0.7,0.7,0.7);

		int i;
		for(i = 0; i < num_lights; i++){
			color += computePlight(lights[i], eye_direction, i);
	    }
	    color = materialDiffuseColor * color;//apply Diffuse tex
 		
	    //apply toon outline
	    if(toon_shading != 0){
			float intensity = (color.x + color.y+color.z )/3;
			gl_FragColor = vec4((dot(eye_direction, world_normal) > 0.25 || dot(eye_direction, world_normal) < -0.25 || intensity > 0.4)? color: vec3(0,0,0),1);
		}else{
			gl_FragColor = vec4(color,1);
		}
		//gl_FragColor = vec3(1,1,1);
	}else{
		gl_FragColor = vec4(in_color,1);
	}
	if(dot(world_normal, eye_direction) < -0.02) gl_FragColor += vec4(0.06,0.06,0.05, 0);
}
