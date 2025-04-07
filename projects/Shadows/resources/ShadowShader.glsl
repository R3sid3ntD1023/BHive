#type vertex
#version 460 core

#include <Skinning.glsl>
#include <Core.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std430, binding = 0) uniform Camera
{
	mat4 uProjection;
	mat4 uView;
	vec2 uNearFar;
	vec3 uCameraPosition;
};

struct PerObjectData
{ 
	mat4 WorldMatrix;
};


layout(std430, binding = 1) restrict readonly buffer ObjectSSBO
{
	PerObjectData o[];
};

layout(std430, binding = 2) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};

layout(location  = 0) out struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
} vs_out;

void main()
{
	vec4 pos = vec4(vPosition , 1);
	mat4 boneTransform = Skinning(vWeights, vBoneIds);
	vec4 posL = boneTransform * pos;

	bool instanced = gl_InstanceIndex != -1; 

	mat4 instance = mix(mat4(1), instances[gl_InstanceIndex], float(instanced));

	mat4 model =  o[gl_DrawID].WorldMatrix * instance;
	gl_Position = uProjection * uView *  model *  posL;

	vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
	vs_out.Position = vec3(model * posL);
}

#type fragment
#version 460 core

#include <Core.glsl>

layout(location  = 0) in struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
} vs_in;

layout(binding = 0) uniform sampler2DArrayShadow uDirectionalShadowMaps;
layout(binding = 1) uniform samplerCubeArray uPointShadowMaps;
layout(binding = 2) uniform sampler2DArray uSpotShadowMaps;

layout(location = 0) out vec4 fColor;


void main()
{
	vec3 P = vs_in.Position;
	vec3 N = normalize(vs_in.Normal);

	vec3 color = vec3(.5) ;
	fColor = vec4(color, 1);
}