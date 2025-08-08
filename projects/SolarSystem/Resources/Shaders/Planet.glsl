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

#include <Core.glsl>
#include <CommonParamsFrag.glsl>
#include <Lighting.glsl>
#include <LambertMaterial.glsl>
#include <LightingParamsFrag.glsl>
#include <DiffuseMapFrag.glsl>


layout(location = 0) out vec4 fColor;

void main()
{
	vec4 diffuseColor = constants.u_Material.DiffuseColor;
	vec3 totalEmissiveRadiance = constants.u_Material.EmissionColor;

	#include <UseDiffuseMapFrag.glsl>

	LambertMaterial material;
	material.DiffuseColor = diffuseColor;
	material.EmissionColor = totalEmissiveRadiance;

	ReflectedLight reflected_light = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	
	#include <CalculateNormals.glsl>
	#include <LightingFrag.glsl>
	
	vec3 out_color = reflected_light.DirectDiffuse + totalEmissiveRadiance;

	#include <OutgoingFragment.glsl>
}
