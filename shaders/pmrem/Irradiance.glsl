#type vertex
#version 460 core

#include <CommonVert.glsl>

#type fragment
#version 460 core

#include <PMREMFuncs.glsl>

layout(location = 0) in struct vertex_output
{
	vec3 position;
} vs_in;

layout(set = 1, binding = 0) uniform samplerCube environmentMap;

layout(location = 0) out vec4 fs_out;

void main()
{
	vec3 normal = normalize(vs_in.position);

	vec3 irradiance = vec3(0);
	vec3 up = vec3(0, 1, 0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	float samplesDelta = 0.025;
	float nrSamples = 0;
	for(float phi = 0; phi < 2.0 * PI; phi += samplesDelta)
	{
		for(float theta = 0; theta < 0.5 * PI; theta += samplesDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	fs_out = vec4(irradiance, 1);
}