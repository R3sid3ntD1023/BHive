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
	#include <includes/Common.vert>
	
	
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

#include <Core.glsl>
#include <Lighting.glsl>
#include <LambertMaterial.glsl>

layout(binding = 0) uniform sampler2D DiffuseMap;
//#include <shadow_passes/Shadow.frag>

layout(push_constant) uniform PushConstants
{
	vec4 DiffuseColor;
	vec3 EmissiveColor;
} constants;

layout(location = 0) in struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
} vs_in;


layout(location = 0) out vec4 fs_out;

void main()
{
	vec4 diffuseColor = constants.DiffuseColor;
	vec3 totalEmissiveRadiance = constants.EmissiveColor;
	vec2 texCoord = vs_in.Texcoord;
	vec3 normal = vs_in.Normal;

	#include <maps/DiffuseMap.glsl>

	LambertMaterial material;
	material.DiffuseColor = diffuseColor;

	ReflectedLight reflected_light = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	
	#include <LightingFragBegin.glsl>
	#include <LightingFragEnd.glsl>
	
	vec3 out_color = reflected_light.DirectDiffuse + totalEmissiveRadiance;

	#include <OutgoingFragment.glsl>
}
