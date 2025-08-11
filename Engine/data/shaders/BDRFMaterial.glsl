#type vertex
#version 460 core


#include <Core.glsl>
#include <CommonParamsVert.glsl>
#include <Skinning.glsl>

void main()
{
	#include <CommonVert.glsl>
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


#include <Core.glsl>
#include <Lighting.glsl>
#include <CommonParamsFrag.glsl>
#include <BDRFFunctions.glsl>
#include <StandardMaterial.glsl>


layout(push_constant) uniform PushConstants
{
	vec3 Albedo;
	vec3 Emission;	
	float Roughness;
	float Metalness;
	float Opacity;
	vec2 Tiling;
	float DepthScale;
	uint Flags;
} constants;

//layout(binding = 3) uniform sampler2DArrayShadow u_shadow_map;
//layout(binding = 4) uniform sampler2DArrayShadow u_shadow_spot_map;
//layout(binding = 5) uniform samplerCubeArrayShadow u_shadow_point_map;


#include <map_params/DiffuseMapParam.glsl>
#include <map_params/RoughnessMapParam.glsl>
#include <map_params/MetalnessMapParam.glsl>
#include <map_params/NormalMapParam.glsl>
#include <map_params/EmissionMapParam.glsl>
#include <map_params/OpacityMapParam.glsl>
#include <map_params/EnvironmentMapsParam.glsl>

#define SHADOW_MAP_BINDING 7

#define HAS_NORMAL_MAP 1 << 0
#define IS_DIAELECTRIC 1 << 1

layout(location = 0) out vec4 fs_out;

void main()
{
	vec4 diffuseColor = vec4(constants.Albedo, constants.Opacity);
	vec3 totalEmissiveRadiance = constants.Emission;
	vec2 texCoord = vs_in.Texcoord * constants.Tiling;
	float roughness = constants.Roughness;
	float metalness = constants.Metalness;
	vec3 normal = normalize(vs_in.Normal);
	
	#include <maps/DiffuseMap.glsl>
	#include <maps/NormalMap.glsl>
	#include <maps/RoughnessMap.glsl>
	#include <maps/MetalnessMap.glsl>
	#include <maps/EmissionMap.glsl>
	#include <maps/OpacityMap.glsl>

	bool isDielectric = (constants.Flags & (IS_DIAELECTRIC)) != 0;
	vec3 F0 = isDielectric ? vec3(0.04) : diffuseColor.rgb;
	F0 =  mix(F0, diffuseColor.rgb, metalnessFactor);


	StandardMaterial material;
	material.Albedo = diffuseColor.rgb;
	material.Metalness = metalnessFactor;
	material.Roughness = roughnessFactor;
	material.IrradianceF0 = F0;
	
	ReflectedLight reflected_light = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));

	#include <LightingFragBegin.glsl>
	#include <maps/EnvironmentMaps.glsl>
	#include <LightingFragEnd.glsl>
	

	vec3 out_color = reflected_light.DirectDiffuse + reflected_light.DirectSpecular + reflected_light.IndirectDiffuse + reflected_light.IndirectSpecular + totalEmissiveRadiance ;

	#include <OutgoingFragment.glsl>
}

