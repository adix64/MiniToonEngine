#version 330

//@@@@@ IN @@@@@
in vec3 in_position;
in vec2 TexCoord;
in vec3 in_normal;
in vec3 in_tangent;
in vec4 BoneIDs;
in vec4 Weights;

//@@@@@ OUT @@@@@
out vec3 world_pos;
out vec3 world_tangent;
out vec3 world_normal;
out vec2 texcoord;

//@@@@@ UNIFORMS @@@@@
uniform mat2x4 boneDQ[128];
uniform mat4 Model;
//uniform mat4 skinning_matrices[100];


mat4 dualQuatToMatrix(mat2x4 dq)
{
	mat4 M;
	float len2 = dot(dq[0], dq[0]);
	float w = dq[0][0], x = dq[0][1], y = dq[0][2], z = dq[0][3];
	float t0 = dq[1][0], t1 = dq[1][1], t2 = dq[1][2], t3 = dq[1][3];
	M[0][0] = w*w + x*x - y*y - z*z;
	M[0][1] = 2 * x * y + 2 * w * z;
	M[0][2] = 2 * x * z - 2 * w * y;
	M[0][3] = 0;
	M[1][0] = 2 * x * y - 2 * w * z;
	M[1][1] = w * w + y * y - x * x - z * z;
	M[1][2] = 2 * y * z + 2 * w * x;
	M[1][3] = 0;
	M[2][0] = 2 * x * z + 2 * w * y;
	M[2][1] = 2 * y * z - 2 * w * x;
	M[2][2] = w * w + z * z - x * x - y * y;
	M[2][3] = 0;
	M[3][0] = -2 * t0 * x + 2 * w * t1 - 2 * t2 * z + 2 * y * t3;
	M[3][1] = -2 * t0 * y + 2 * t1 * z - 2 * x * t3 + 2 * w * t2;
	M[3][2] = -2 * t0 * z + 2 * x * t2 + 2 * w * t3 - 2 * t1 * y;
	M[3][3] = len2;
	M /= len2;
	return M;
}

void DualQuaternionSkinning()
{
	float yc = 1.0, zc = 1.0, wc = 1.0;
	if (dot(boneDQ[int(BoneIDs[0])][0], boneDQ[int(BoneIDs[1])][0]) < 0.0)
		yc = -1.0;
	if (dot(boneDQ[int(BoneIDs[0])][0], boneDQ[int(BoneIDs[2])][0]) < 0.0)
		zc = -1.0;
	if (dot(boneDQ[int(BoneIDs[0])][0], boneDQ[int(BoneIDs[3])][0]) < 0.0)
		wc = -1.0;
	
	mat2x4 dq0 = mat2x4(boneDQ[int(BoneIDs[0])][0], boneDQ[int(BoneIDs[0])][1]);
	mat2x4 dq1 = mat2x4(boneDQ[int(BoneIDs[1])][0], boneDQ[int(BoneIDs[1])][1]);
	mat2x4 dq2 = mat2x4(boneDQ[int(BoneIDs[2])][0], boneDQ[int(BoneIDs[2])][1]);
	mat2x4 dq3 = mat2x4(boneDQ[int(BoneIDs[3])][0], boneDQ[int(BoneIDs[3])][1]);

	if (dot(dq0[0], dq1[0]) < 0.0) dq1 *= -1.0;
	if (dot(dq0[0], dq2[0]) < 0.0) dq2 *= -1.0;
	if (dot(dq0[0], dq3[0]) < 0.0) dq3 *= -1.0;

	mat2x4 blendDQ = Weights[0]*dq0;
	blendDQ += Weights[1]*dq1;
	blendDQ += Weights[2]*dq2;
	blendDQ += Weights[3]*dq3;


	
	mat4 skinTransform = dualQuatToMatrix(blendDQ);
	
	vec4 blendVertex = skinTransform*vec4(in_position,1);
	vec4 blendNormal = skinTransform*vec4(in_normal,0);
	vec4 blendTangent = skinTransform*vec4(in_tangent, 0);

	world_pos = (Model * blendVertex).xyz;
	world_normal = (Model * normalize(blendNormal)).xyz;
	world_tangent = (Model * normalize(blendTangent)).xyz;
}

/*
// PRODUCES CANDY WRAPPER ARTIFACTS...
void matrix_pallete_skinning(){
	mat4 BoneTransform = skinning_matrices[int(BoneIDs[0])] * Weights[0] +
    				skinning_matrices[int(BoneIDs[1])] * Weights[1] + 
    				skinning_matrices[int(BoneIDs[2])] * Weights[2] +
    				skinning_matrices[int(BoneIDs[3])] * Weights[3];

	mat4 scalemat = mat4(0.0328084,0,0,0, 0,0.0328084,0,0, 0,0,0.0328084,0, 0,0,0,1);

	world_pos = vec3(Model * scalemat * BoneTransform * vec4(in_position,1));
	world_normal = normalize(vec3(Model * BoneTransform * vec4(in_normal,0)));
}
*/

void main()
{
	texcoord = TexCoord;
	DualQuaternionSkinning();
}
