#type compute
#version 460 core

#include <PMREMFuncs.glsl>

#define LOCAL_SIZE 8

layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;

// One view per mip is bound from the CPU side:
//   SetTexture("imgOutput", mPreFilteredEnvironment, mip);
// so this image always targets a single mip level.
layout(rgba16f, set = 1, binding = 0) uniform restrict writeonly imageCube imgOutput;

// Full environment cube with all mips.
layout(set = 1, binding = 1) uniform samplerCube environmentMap;


layout(push_constant) uniform PushConstants
{
	float u_roughness;
	int u_mip_level;
	uint u_width;
	uint u_height;
	uint u_envResolution;
} constants;


void main()
{
	// Face index is in workgroup Z, we dispatch with z = 6
    uint face  = gl_GlobalInvocationID.z;
    uint x     = gl_GlobalInvocationID.x;
    uint y     = gl_GlobalInvocationID.y;

    // Bounds check for the current mip resolution
    if (x >= constants.u_width || y >= constants.u_height || face >= 6u)
        return;

	vec3 N = CalculateDirection(int(face), int(x), int(y), float(constants.u_width), float(constants.u_height));
	vec3 R = N;
	vec3 V = R;
	float roughness = constants.u_roughness;

	const uint SAMPLE_COUNT = 1024u;
	float weight = 0.0;
	vec3 prefilterColor = vec3(0);
	float resolution = float(constants.u_envResolution);
	float saTexel = 4.0 * PI / (6.0 * resolution * resolution);

	for(uint i = 0u; i < SAMPLE_COUNT; i++)
	{
		vec2 Xi = Hamemersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi, N, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0);
		if(NdotL > 0.0)
		{
			float NdotH  = max(dot(N, H), 0.0);
			float HdotV  = max(dot(H, V), 0.0);

			float D = DisributionGXX(N, H, roughness);
			float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mip_offset = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
			float lod = float(constants.u_mip_level) + mip_offset;
			prefilterColor += textureLod(environmentMap, L, lod).rgb * NdotL;

			weight += NdotL;
		}
	}

	prefilterColor /= max(weight, 1e-4);
    
	ivec3 coord = ivec3(int(x), int(y), int(face));
	imageStore(imgOutput, coord, vec4(prefilterColor, 1));
}