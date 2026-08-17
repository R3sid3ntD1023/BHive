#type vertex
#version 460 core

#include <ForwardMesh.vert>

#type fragment
#version 460 core

#include <Core.glsl>
#include <Lighting.glsl>

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
	float InstanceID;
	float DrawID;
} vs_in;

struct LambertMaterial
{
	vec4 Diffuse;
	vec3 Emission;
	float Opacity;
};

layout(push_constant) uniform MaterialInfo
{
	vec4 DiffuseColor;
	vec4 Emission;
} pc;

layout(set = 1, binding = 0) uniform sampler2D DiffuseMap;
layout(set = 1, binding = 1) uniform sampler2D EmissionMap;

LambertMaterial GetMaterial(VS_OUT vs)
{
	vec4 diffuseColor = pc.DiffuseColor;
	vec3 totalEmissiveRadiance = pc.Emission.rgb * pc.Emission.a;
	vec2 texCoord = vs_in.Texcoord;

	#include <DiffuseMap.glsl>
	#include <EmissionMap.glsl>

	LambertMaterial mat;
	mat.Diffuse = diffuseColor;
	mat.Emission = totalEmissiveRadiance;
	mat.Opacity = diffuseColor.a;

	return mat;
}

void Direct_Lambert(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight directLight, const in LambertMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.Diffuse.rgb; 
	vec3 irradiance = max(dot(geoNormal, directLight.Direction), 0.0) * diffuse;
	reflectedLight.DirectDiffuse += irradiance * directLight.Color;
}

#define Direct Direct_Lambert
#define Material LambertMaterial
#define HAS_EMISSION
#define HAS_OPACITY

#include <ForwardMesh.frag>