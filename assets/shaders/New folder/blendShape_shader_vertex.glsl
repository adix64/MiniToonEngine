#version 120

/*layout (location = 0) inattribute*/  in vec3 in_position_a1;
/*layout (location = 1) inattribute*/  in vec3 in_position_a2;
/*layout (location = 2) inattribute*/  in vec3 in_position_b1;
/*layout (location = 3) inattribute*/  in vec3 in_position_b2;
/*layout (location = 4) inattribute*/  in vec2 TexCoord;
/*layout (location = 5) inattribute*/  in vec3 in_normal_a1;
/*layout (location = 6) inattribute*/  in vec3 in_normal_a2;
/*layout (location = 7) inattribute*/  in vec3 in_normal_b1;
/*layout (location = 8) inattribute*/  in vec3 in_normal_b2;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float fact_a12, fact_b12, blend_fact;
uniform int blend_animations;

/*out*/varying vec3 world_pos;
/*out*/varying vec3 world_normal;
/*out*/varying vec2 texcoord;

void main()
{	

	texcoord = TexCoord;
	vec3 pos = (1- fact_a12) * in_position_a1 + fact_a12 * in_position_a2; 
	vec3 normal = normalize((1 - fact_a12) * in_normal_a1 + fact_a12 * in_normal_a2);
	if(blend_animations != 0){
		pos = (1-blend_fact) * pos + blend_fact * ((1- fact_b12) * in_position_b1 + fact_b12 * in_position_b2);
		normal = normalize((1-blend_fact) * normal +  blend_fact * normalize((1-fact_b12) * in_normal_b1 + fact_b12 * in_normal_b2));
	}
//	pos = in_position_a1;
	mat4 mModel = Model;
	//mModel[3][1]= 100;
	world_pos = (mModel * vec4(pos, 1)).xyz;

	world_normal = normalize((mModel * vec4(normal,0)).xyz);
	world_pos = world_pos + world_normal * 0.05;
	gl_Position = Projection * View * vec4(world_pos,1);
}
