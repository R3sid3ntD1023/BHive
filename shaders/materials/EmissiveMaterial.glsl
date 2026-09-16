#type vertex
#version 460 core

#include <ForwardMesh.vert>

#type fragment
#version 460 core

#include <Core.glsl>
#include <Lighting.glsl>

#define USE_EMISSION_MAP

layout(location = 0) in struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
	vec3 DebugColor;
} vs_in;

struct EmissiveMaterial
{
	vec3 Emission;
	vec3 Normal;
};

layout(push_constant) uniform MaterialInfo
{
	vec4 Emission;
} pc;

layout(set = 1, binding = 0) uniform sampler2D EmissionMap;

EmissiveMaterial GetMaterial(VS_OUT vs)
{
	vec2 texCoord = vs.Texcoord;
	vec3 totalEmissiveRadiance = pc.Emission.rgb * pc.Emission.a;
	vec3 normal = normalize(vs_in.Normal);

	#include <EmissionMap.glsl>

	EmissiveMaterial mat;
	mat.Emission = totalEmissiveRadiance;
	mat.Normal = normal;

	return mat;
}

#define HAS_EMISSION
#define Material EmissiveMaterial


#include <ForwardMesh.frag>