#type compute
#version 460 core

#include <PMREMFuncs.glsl>

layout (local_size_x = 8, local_size_y = 8, local_size_z = 6) in;

layout(rgba32f, set = 1, binding = 0) uniform restrict writeonly imageCube irradianceMap;
layout(set = 1, binding = 1) uniform samplerCube environmentMap;

layout(push_constant) uniform PushConstants
{
	uint u_width;
	uint u_height;
} pc;

void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint face = gl_GlobalInvocationID.z;

	vec3 N = CalculateDirection(face, x, y, float(pc.u_width), float(pc.u_height));
	N = normalize(N);

	vec3 irradiance = vec3(0);
	vec3 up = vec3(0, 1, 0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float samplesDelta = 0.025;
	float nrSamples = 0;
	for(float phi = 0; phi < 2.0 * PI; phi += samplesDelta)
	{
		for(float theta = 0; theta < 0.5 * PI; theta += samplesDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	imageStore(irradianceMap, ivec3(x, y, face), vec4(irradiance, 1.0));
}