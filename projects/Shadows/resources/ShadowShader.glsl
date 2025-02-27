#type vertex
#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require
#extension GL_ARB_shading_language_include : require

#include <Skinning.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std430, binding = 0) uniform Camera
{
	mat4 uProjection;
	mat4 uView;
	vec2 uNearFar;
};

#define INSTANCED_FLAG 0x01

struct PerObjectData
{ 
	mat4 WorldMatrix;
};


layout(std430, binding = 0) restrict readonly buffer ObjectSSBO
{
	PerObjectData o[];
};

layout(std430, binding = 1) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};

layout(location  = 0) out struct VS_OUT
{
	vec3 Normal;
} vs_out;

void main()
{

	vec4 pos = vec4(vPosition , 1);
	mat4 boneTransform = Skinning(vWeights, vBoneIds);
	vec4 posL = boneTransform * pos;

	bool instanced = gl_InstanceID != 0; 

	mat4 instance = (mat4(1) * (1.f - float(instanced))) +  (instances[gl_InstanceID] *  float(instanced));

	mat4 model =  instance * o[gl_DrawID].WorldMatrix ;
	gl_Position = uProjection * uView *  model *  posL;

	vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
}

#type fragment
#version 460 core

vec3 lightDir = vec3(1, -.3, -.5);


layout(location  = 0) in struct VS_OUT
{
	vec3 Normal;
} vs_in;

layout(location = 0) out vec4 fColor;

void main()
{
	float ndotl = max(dot(normalize(vs_in.Normal), -lightDir), 0.0);
	vec3 color = vec3(.5) * ndotl;
	fColor = vec4(color, 1);
}