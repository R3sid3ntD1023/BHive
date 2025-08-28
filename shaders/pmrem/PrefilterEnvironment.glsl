#type compute
#version 460 core

#include <PMREMFuncs.glsl>

#define LOCAL_SIZE 8

layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;

layout(rgba16f, binding = 0) uniform restrict writeonly imageCube imgOutput;
layout(binding = 0) uniform samplerCube environmentMap;


layout(push_constant) uniform PushConstants
{
	float u_roughness;
	int u_mip_level;
	uint u_width;
	uint u_height;
} constants;


void main()
{
	vec3 N = CalculateDirection(gl_GlobalInvocationID.z, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, float(constants.u_width), float(constants.u_height));
	vec3 R = N;
	vec3 V = R;
	float roughness = constants.u_roughness;
	int current_mip_level = constants.u_mip_level;

	const uint SAMPLE_COUNT = 1024u;
	float weight = 0.0;
	vec3 prefilterColor = vec3(0);
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

			float resolution = 512.0;
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mip_level = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
			prefilterColor += textureLod(environmentMap, L, current_mip_level + mip_level).rgb * NdotL;

			weight += NdotL;
		}
	}

	prefilterColor /= weight;
    

	imageStore(imgOutput, ivec3(gl_GlobalInvocationID), vec4(prefilterColor, 1));
}