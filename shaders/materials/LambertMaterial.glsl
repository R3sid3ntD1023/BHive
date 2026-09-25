#type vertex
#version 460 core

#include <ForwardMesh.vert>

#type fragment
#version 460 core

#include <Core.glsl>
#include <Constants.glsl>
#include <Lighting.glsl>
#include <ShadowBuffer.glsl>


layout(set = 0, binding = 6) uniform sampler2DArrayShadow ShadowDirMaps;
layout(set = 0, binding = 7) uniform samplerCubeArrayShadow ShadowPointMaps;
layout(set = 0, binding = 8) uniform sampler2DArrayShadow ShadowSpotMaps;

#define USE_DIFFUSE_MAP
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
	mat4 View;
} vs_in;

struct LambertMaterial
{
	vec4 Diffuse;
	vec3 Emission;
	vec3 Normal;
	float Opacity;
};

layout(push_constant) uniform MaterialInfo
{
	vec4 DiffuseColor;
	vec4 Emission;
} pc;

layout(set = SET_MATERIAL, binding = 0) uniform sampler2D DiffuseMap;
layout(set = SET_MATERIAL, binding = 1) uniform sampler2D EmissionMap;

LambertMaterial GetMaterial(VS_OUT vs)
{
	vec4 diffuseColor = pc.DiffuseColor;
	vec3 totalEmissiveRadiance = pc.Emission.rgb * pc.Emission.a;
	vec2 texCoord = vs_in.Texcoord;
	vec3 normal = normalize(vs_in.Normal);

	#include <DiffuseMap.glsl>
	#include <EmissionMap.glsl>

	LambertMaterial mat;
	mat.Diffuse = diffuseColor;
	mat.Emission = totalEmissiveRadiance;
	mat.Opacity = diffuseColor.a;
	mat.Normal = normal;

	return mat;
}

void Direct_Lambert(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight directLight, const in LambertMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.Diffuse.rgb; 
	vec3 irradiance = max(dot(geoNormal, directLight.Direction), 0.0) * diffuse;
	reflectedLight.DirectDiffuse += irradiance * directLight.Color;
}

#define DIRECTIONAL_SHADOW_MAPPING
#define POINT_SHADOW_MAPPING
#define SPOT_SHADOW_MAPPING
#define Direct Direct_Lambert
#define Material LambertMaterial
#define HAS_EMISSION
#define HAS_OPACITY

#include <ForwardMesh.frag>