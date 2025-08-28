#type vertex
#version 460 core

#include <CommonVert.glsl>

#type fragment
#version 460 core

#include <PMREMFuncs.glsl>

layout(location = 0) in struct vertex_out
{
	vec3 position;
} vs_in;

layout(binding =  0) uniform sampler2D equirectangularMap;

layout(location =  0 ) out vec4 fs_out;

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
	vec2 uv = SampleSphericalMap(normalize(vs_in.position));
	vec3 color = texture(equirectangularMap, uv).rgb;
	color = ACES(color);

	fs_out = vec4(color, 1);
}