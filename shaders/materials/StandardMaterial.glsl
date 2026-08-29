#type vertex
#version 460 core

#include <ForwardMesh.vert>

#type fragment
#version 460 core

#include <Core.glsl>
#include <Lighting.glsl>

#define USE_DIFFUSE_MAP
#define USE_ROUGHNESS_MAP
#define USE_METALNESS_MAP
#define USE_NORMAL_MAP
#define USE_EMISSION_MAP
#define USE_OPACITY_MAP
#define USE_ENVIRONMENT_MAPS

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
	vec3 DebugColor;
} vs_in;

layout(push_constant) uniform MaterialInfo
{
	vec3 Albedo;
	vec4 Emission;
	float Roughness;
	float Metalness;
	float Opacity;
	vec2 Tiling;
	uint HasNormalMap;
	uint Flags;
} pc;

struct StandardMaterial
{
	vec3 Albedo;
	vec3 Emission;
	float Roughness;
	float Metalness;
	float Opacity;
	vec3 IrradianceF0;
};

// @semantic EnvironmentBRDFLUT
layout(set = 0, binding = 2) uniform sampler2D BRDFLutMap;

// @semantic EnvironmentPreFilter
layout(set = 0, binding = 3) uniform samplerCube PreFilterMap;

// @semantic EnvironmentIrradiance
layout(set = 0, binding = 4) uniform samplerCube IrradianceMap;

layout(set = 1, binding = 0) uniform sampler2D DiffuseMap;
layout(set = 1, binding = 1) uniform sampler2D NormalMap;
layout(set = 1, binding = 2) uniform sampler2D RoughnessMap;
layout(set = 1, binding = 3) uniform sampler2D MetalnessMap;
layout(set = 1, binding = 4) uniform sampler2D EmissionMap;
layout(set = 1, binding = 5) uniform sampler2D OpacityMap;

#define RECEIVE_SHADOWS 1 << 1
#define IS_DIAELECTRIC 1 << 2

StandardMaterial GetMaterial(VS_OUT vs)
{
	bool ReceiveShadows = (pc.Flags & RECEIVE_SHADOWS) != 0;
	bool IsDielectric = (pc.Flags & IS_DIAELECTRIC) != 0;
	bool HasNormalMap = pc.HasNormalMap != 0;

	vec4 diffuseColor = vec4(pc.Albedo, pc.Opacity);
	vec3 totalEmissiveRadiance = pc.Emission.rgb * pc.Emission.a;
	vec2 texCoord = vs_in.Texcoord * pc.Tiling;
	float roughness = pc.Roughness;
	float metalness = pc.Metalness;
	vec3 normal = normalize(vs_in.Normal);
	
	#include <DiffuseMap.glsl>
	#include <NormalMap.glsl>
	#include <RoughnessMap.glsl>
	#include <MetalnessMap.glsl>
	#include <EmissionMap.glsl>
	#include <OpacityMap.glsl>

	vec3 F0 = IsDielectric ? vec3(0.04) : diffuseColor.rgb;
	F0 =  mix(F0, diffuseColor.rgb, metalnessFactor);

	StandardMaterial mat;
	mat.Albedo = diffuseColor.rgb;
	mat.Roughness = roughness;
	mat.Metalness = metalness;
	mat.Emission  = totalEmissiveRadiance;
	mat.Opacity = diffuseColor.a;
	mat.IrradianceF0 = F0;

	return mat;
}

#include <BDRFFunctions.glsl>

void Direct_Standard(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight light, const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 bdrf = CalculateBDRF(material.IrradianceF0, geoNormal, geoViewDir, light.Direction, material.Albedo, material.Metalness, material.Roughness);

	float NdotL = max(dot(geoNormal, light.Direction), 0.0);
	reflectedLight.DirectDiffuse += (bdrf * light.Color * NdotL);
}

void InDirect_Standard(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir,const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 F = fresnelSchlickRoughness(max(dot(geoNormal,geoViewDir), 0.0), material.IrradianceF0, material.Roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.Metalness;
	reflectedLight.IndirectDiffuse = kD *  irradiance * material.Albedo;
}

void InDirectSpecular_Standard(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	reflectedLight.IndirectSpecular =  irradiance;
}

#define Direct Direct_Standard
#define InDirect InDirect_Standard
#define InDirectSpecular InDirectSpecular_Standard
#define Material StandardMaterial
#define HAS_EMISSION
#define HAS_OPACITY

#include <ForwardMesh.frag>
