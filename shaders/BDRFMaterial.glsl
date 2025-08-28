#type vertex
#version 460 core


#include <Core.glsl>
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
	vec3 u_camera_position;
};

layout(std430, binding = 1) restrict readonly buffer PerObjectSSBO
{
	PerObjectData object[];
};


layout(std430, binding = 2) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};

layout(std430, binding = 3) restrict readonly buffer Bones
{
	mat4 bones[MAX_BONES];
};


layout(location = 0) out struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
} vs_out;

void main()
{
	#include <Common.vert>

		
	vs_out.Position = worldPos.xyz;
	vs_out.TBN = mat3(T, B, N);
	vs_out.Texcoord = vTexCoord;
	vs_out.Normal = N;
	vs_out.CameraPosition = u_camera_position;
	vs_out.Color = vColor;

}

#type fragment
#version 460 core

#define USE_DIFFUSE_MAP
#define USE_ROUGHNESS_MAP
#define USE_METALNESS_MAP
#define USE_NORMAL_MAP
#define USE_EMISSION_MAP
#define USE_OPACITY_MAP
#define USE_ENVIRONMENT_MAPS


layout(binding = 0) uniform sampler2D DiffuseMap;
layout(binding = 1) uniform sampler2D NormalMap;
layout(binding = 2) uniform sampler2D RoughnessMap;
layout(binding = 3) uniform sampler2D MetalnessMap;
layout(binding = 4) uniform sampler2D EmissionMap;
layout(binding = 5) uniform sampler2D OpacityMap;
layout(binding = 6) uniform samplerCube PreFilterMap;
layout(binding = 7) uniform samplerCube IrradianceMap;
layout(binding = 8) uniform sampler2D BRDFLutMap;
#include <shadow_passes/Shadow.frag>

#include <Core.glsl>
#include <Lighting.glsl>
#include <StandardMaterial.glsl>


layout(location = 0) in struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
} vs_in;


layout(push_constant) uniform PushConstants
{
	vec3 Albedo;
	vec3 Emission;	
	float Roughness;
	float Metalness;
	float Opacity;
	vec2 Tiling;
	float DepthScale;
	uint HasNormalMap;
	uint Flags;
} constants;


#define RECEIVE_SHADOWS 1 << 1
#define IS_DIAELECTRIC 1 << 2

layout(location = 0) out vec4 fs_out;

void main()
{
	bool ReceiveShadows = (constants.Flags & RECEIVE_SHADOWS) != 0;
	bool IsDielectric = (constants.Flags & IS_DIAELECTRIC) != 0;
	bool HasNormalMap = constants.HasNormalMap != 0;

	vec4 diffuseColor = vec4(constants.Albedo, constants.Opacity);
	vec3 totalEmissiveRadiance = constants.Emission;
	vec2 texCoord = vs_in.Texcoord * constants.Tiling;
	float roughness = constants.Roughness;
	float metalness = constants.Metalness;
	vec3 normal = normalize(vs_in.Normal);
	
	#include <DiffuseMap.glsl>
	#include <NormalMap.glsl>
	#include <RoughnessMap.glsl>
	#include <MetalnessMap.glsl>
	#include <EmissionMap.glsl>
	#include <OpacityMap.glsl>

	

	vec3 F0 = IsDielectric ? vec3(0.04) : diffuseColor.rgb;
	F0 =  mix(F0, diffuseColor.rgb, metalnessFactor);


	StandardMaterial material;
	material.Albedo = diffuseColor.rgb;
	material.Metalness = metalnessFactor;
	material.Roughness = roughnessFactor;
	material.IrradianceF0 = F0;
	
	ReflectedLight reflected_light = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));

	#include <LightingFragBegin.glsl>
	#include <EnvironmentMaps.glsl>
	#include <LightingFragEnd.glsl>
	

	vec3 out_color = reflected_light.DirectDiffuse + reflected_light.DirectSpecular + reflected_light.IndirectDiffuse + reflected_light.IndirectSpecular + totalEmissiveRadiance ;

	#include <OutgoingFragment.glsl>
}

