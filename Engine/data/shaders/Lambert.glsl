#type vertex

#version 460 core

#include <Core.glsl>
#include <CommonParamsVert.glsl>

void main()
{
	#include <CommonVert.glsl>
}

#type fragment

#version 460 core

#define USE_DIFFUSE_MAP

#include <Core.glsl>
#include <CommonParamsFrag.glsl>
#include <Lighting.glsl>
#include <LambertMaterial.glsl>
#include <map_params/DiffuseMapParam.glsl>

layout(push_constant) uniform PushConstants
{
	vec4 DiffuseColor;
	vec3 EmissiveColor;
} constants;

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
