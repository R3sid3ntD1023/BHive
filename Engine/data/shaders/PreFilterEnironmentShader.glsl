#type compute
#version 460 core

#define LOCAL_SIZE 8
#define POS_X 0
#define NEG_X 1
#define POS_Y 2
#define NEG_Y 3
#define POS_Z 4
#define NEG_Z 5

layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;


layout(rgba16f, binding = 0) uniform restrict writeonly imageCube imgOutput;
layout(binding = 0) uniform samplerCube environmentMap;

layout(location = 0) uniform float u_roughness;
layout(location = 1) uniform int u_mip_level;
layout(location = 2) uniform uint u_width;
layout(location = 3) uniform uint u_height;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hamemersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
vec3 CalculateDirection(uint face, uint face_x, uint face_y, float w, float h);
float DisributionGXX(vec3 N, vec3 H, float roughness);
float unlerp(float val, float max_val);

void main()
{
    vec3 N = CalculateDirection(gl_GlobalInvocationID.z, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, float(u_width), float(u_height));
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float weight = 0.0;
    vec3 prefilterColor = vec3(0);
    for(uint i = 0u; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = Hamemersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            float NdotH  = max(dot(N, H), 0.0);
            float HdotV  = max(dot(H, V), 0.0);

            float D = DisributionGXX(N, H, u_roughness);
            float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

            float resolution = 512.0;
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mip_level = u_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            prefilterColor += textureLod(environmentMap, L, u_mip_level + mip_level).rgb * NdotL;

            weight += NdotL;
        }
    }

    prefilterColor /= weight;
    

    imageStore(imgOutput, ivec3(gl_GlobalInvocationID), vec4(prefilterColor, 1));
}

float DisributionGXX(vec3 N, vec3 H, float roughness)
{
	float a2 = roughness * roughness * roughness * roughness;
	float ndoth = max(dot(N, H), 0.0);
	float ndoth2 = ndoth * ndoth;

	float denom = (ndoth2 * (a2 - 1.0) + 1.0);
	return a2 /  (PI * denom * denom) + 0.0001;

}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0x0F0F0F0Fu) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits)  * 2.3283064365386963e-10;
}

vec2 Hamemersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0 ,0 ,1) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

//https://github.com/diharaw/runtime-ibl/blob/master/src/shader/prefilter_cs.glsl

float unlerp(float val, float max_val)
{

    return (val + 0.5) / max_val;
}

vec3 CalculateDirection(uint face, uint face_x, uint face_y, float w, float h)
{
    float s = unlerp(float(face_x), w) * 2.0 - 1.0;
    float t = unlerp(float(face_y), h) * 2.0 - 1.0;
    float x, y ,z;

    switch(face)
    {
        case POS_Z:
            x = s;
            y = -t;
            z = 1;
            break;
        case NEG_Z:
            x = -s;
            y = -t;
            z = -1;
            break;
        case NEG_X:
            x = -1;
            y = -t;
            z = s;
            break;
        case POS_X:
            x = 1;
            y = -t;
            z = -s;
            break;
        case POS_Y:
            x = s;
            y = 1;
            z = t;
            break;
        case NEG_Y:
            x = s;
            y = -1;
            z = -t;
            break;
        default:
        break;
    }

    vec3 d;
    float inv_len = 1.0 /sqrt(x * x + y * y + z * z);
    d.x = x * inv_len;
    d.y = y * inv_len;
    d.z = z * inv_len;

    return d;
}
