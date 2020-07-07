#version 330



//@@@@@ IN @@@@@
in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;


float localityPhase;
//uniform mat4 skinning_matrices[100];
uniform mat4 PV;
uniform mat4 Model;
uniform vec3 eye_pos;
uniform float uTime;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
#define PI 3.1415926
void main()
{		 
	vec3 initWorldPos = (Model * vec4(in_position, 1)).xyz;
	float len = length(initWorldPos - eye_pos);
	float screenScale = len / 4;

	localityPhase = float(gl_VertexID);
	
	// float wave1 = abs(0.2 * cos(uTime + localityPhase) + 0.8 * cos(uTime));
	// float periodic = pow(wave1, 0.1618);
	// float wave2 = abs( 0.5 * cos(uTime + localityPhase + PI) + 0.5* cos(uTime+ PI));
	// float periodic2 = pow(wave2, 0.1618);
	float extrudeFact = (0.8382 + 0.1618 * rand(vec2(localityPhase * uTime, localityPhase))) * screenScale;//;
	float extrudeFact2 = (0.8382 + 0.1618 * rand(vec2(localityPhase, localityPhase * uTime))) * screenScale;
	extrudeFact = min(5, extrudeFact);
	extrudeFact2 = min(5, extrudeFact2);
	// float s2 = extrudeFact = extrudeFact * extrudeFact;

	// extrudeFact += 1 + rand(in_position.xy) * extrudeFact;
	// extrudeFact *= 0.;

	vec3 shiftDir = extrudeFact * in_normal * 0.5;
	vec3 biTangent = 0.15 * extrudeFact2 * (0.5 * cross(in_normal, in_tangent) + 0.5 * in_tangent);

	vec3 world_pos = (Model * (vec4(in_position, 1) + vec4(shiftDir,0) + vec4(biTangent,0))).xyz;

	gl_Position = PV * vec4(world_pos,1);
}
