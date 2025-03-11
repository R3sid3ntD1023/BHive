#type vertex

#version 460 core

#include <Core.glsl>
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

layout(location = 0) out struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec3 CameraPosition;
} vs_out;

void main()
{

	vec4 pos = vec4(vPosition, 1);
	mat4 boneTransform = Skinning(vWeights, vBoneIds);
	vec4 posL = boneTransform * pos;

	bool instanced = gl_InstanceID != 0;

	mat4 instance = mix(mat4(1), instances[gl_InstanceID], float(instanced));

	mat4 model = o[gl_DrawID].WorldMatrix * instance;
	vec4 worldPos = model * posL;
	gl_Position = uProjection * uView * worldPos;

	vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
	vs_out.Position = worldPos.xyz;
	vs_out.CameraPosition = uCameraPosition;
}

#type fragment
#version 460 core

layout(location = 0) in struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec3 CameraPosition;
} vs_in;

layout(std430, binding = 1) uniform Material
{
	vec4 Color;
	float IOR;
	float Reflective;
};

layout(location = 0) out vec4 fColor;

void main()
{
	fColor = Color;
}