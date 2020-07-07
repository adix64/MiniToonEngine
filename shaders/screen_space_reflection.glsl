#version 330
 
uniform int wWidth, wHeight;

uniform sampler2D gFinalImage;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gExtraComponents;
uniform sampler2D ColorBuffer; 

// uniform mat4 invView;
uniform mat4 projection;
uniform mat4 invprojection;
uniform mat4 view;

vec2 TexCoords;
vec3 viewNormal;

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 20;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = vec3((view * vec4(texture(gPosition, projectedCoord.xy).xyz, 1))).z;

 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    dir *= step; 
 
    float depth;
    int steps;
    vec4 projectedCoord;

    if(dot(normalize(dir), vec3(0,0,1)) > 0.5)
        return vec4(-1);
    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = vec3((view * vec4(texture(gPosition, projectedCoord.xy).xyz, 1))).z;

        if(depth > 1000.0)
            return vec4(-1);
 
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {   
                vec4 Result;

                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
        }
        
        steps++;
    }
 
    
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}



void main()
{
    TexCoords = gl_FragCoord.xy / vec2(wWidth, wHeight);
    Metallic = 1;//texture2D(gExtraComponents, TexCoords).g;

    if(Metallic < 0.01)
        discard;
 
    viewNormal = vec3(view * vec4(texture2D(gNormal, TexCoords).xyz, 0));// * invView);
    vec3 viewPos = vec3(view * vec4(texture2D(gPosition, TexCoords).xyz, 1));
    vec3 albedo = texture(gFinalImage, TexCoords).rgb;

    float spec = Metallic;//1;//texture(ColorBuffer, TexCoords).w;

    // vec3 F0 = vec3(0.04); 
    // F0      = mix(F0, albedo, Metallic);
    // vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    vec3 hitPos = viewPos;
    float dDepth;
 
    vec3 wp = floor(viewPos * 30);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
    vec4 coords = RayCast(vec3(jitt) + reflected* max(minRayStep, -viewPos.z), hitPos, dDepth);
 
 
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - TexCoords.xy));
 
 
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
                screenEdgefactor * 
                -reflected.z;
    if(coords.z > viewPos.z)
        discard;
    // Get color
    if(coords.x < 0 || coords.x > 1 ||coords.y < 0 || coords.y > 1)
        discard;

    vec3 SSR = texture2D(gFinalImage, coords.xy).rgb;// * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  

    gl_FragColor = vec4(SSR, 1);
}