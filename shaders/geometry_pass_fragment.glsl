#version 330
                                                                        
/*in*/varying vec3 world_pos;
/*in*/varying vec3 world_normal;
	varying vec3 world_tangent;
	varying vec3 world_bitangent;

/*in*/varying vec2 texcoord;
/*in*/varying float depth;

// layout (location = 0) out vec3 gl_FragData[0];
// layout (location = 1) out vec3 gl_FragData[1];
// layout (location = 2) out vec3 gl_FragData[2];
// layout (location = 3) out vec3 gl_FragData[3];

struct Texture{
	int diffuseMapON;
	int emissiveMapON;
	int bumpMapON;
	int specularMapON;
	float cosinePower;
	float specularIntensity;
};

uniform Texture uv_maps;
uniform sampler2D diffuseMap;
uniform sampler2D bumpMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;

uniform int solid_color;
uniform vec3 in_color;
uniform int toon_shading;

uniform int lightEmitter;
uniform vec3 uColor;
uniform float uEmissive;


vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(world_normal);
    vec3 Tangent = normalize(world_tangent);
    // Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(bumpMap, texcoord).xyz;
    BumpMapNormal = vec3(BumpMapNormal.x, 1 - BumpMapNormal.y, BumpMapNormal.z); 
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main()
{
	
	gl_FragData[0] = vec4(world_pos,1);

	if(uv_maps.diffuseMapON == 0)
		gl_FragData[1] = vec4(uColor,1);
	else
	{
		gl_FragData[1] = texture2D(diffuseMap, texcoord);
		if(texture2D(diffuseMap, texcoord).a < 0.2)
			discard;
	}
	// gl_FragData[1] = texture2D(specularMap, texcoord);
	// gl_FragData[1] = vec4(0.5,0.5,0.5,1);

	gl_FragData[2] = vec4(0, 0, 0, 1);


	gl_FragData[3] = vec4(normalize(CalcBumpedNormal()),1);

	float emissiveFactor;
	if(uv_maps.emissiveMapON == 1)
		emissiveFactor = texture2D(emissiveMap, texcoord).x;
	else
		emissiveFactor = uEmissive;
	gl_FragData[4] = vec4(toon_shading, 0, emissiveFactor, 1);
	
	// if(uv_maps.specularMapON != 0)
	// {
	gl_FragData[2] = vec4(texture2D(specularMap, texcoord).xyz,1);
	gl_FragData[4].y = gl_FragData[2].y;


	// }
}