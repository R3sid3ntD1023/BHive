#type compute
#version 460 core

#include <PMREMFuncs.glsl>

layout(local_size_x = 8, local_size_y = 8, local_size_z = 6) in;

layout(rgba32f, set = 1, binding = 0) uniform writeonly imageCube imgOutput;
layout(set = 1, binding = 1) uniform sampler2D equirectangularMap;

layout(push_constant) uniform PushConstants
{
	uint u_width;
	uint u_height;
} pc;

vec2 SampleSphericalMap(vec3 v)
{
	const vec2 invATan = vec2(0.1591, 0.3183);

	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invATan;
	uv += 0.5;
	return uv;
}

vec3 ACES(vec3 color) {
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;
	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint face = gl_GlobalInvocationID.z;

	if(x >= pc.u_width || y >= pc.u_height || face >= 6u)
		return;

	vec3 N = CalculateDirection(face, x, y, float(pc.u_width), float(pc.u_height));
	vec2 uv = SampleSphericalMap(normalize(N));
	vec3 color = texture(equirectangularMap, uv).rgb;
	color = ACES(color);

	imageStore(imgOutput, ivec3(x, y, int(face)), vec4(color, 1.0));
}