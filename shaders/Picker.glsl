#type vertex
#version 460

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


void main()
{
	#include <includes/Common.vert>
}

#type fragment
#version 460

layout(push_constant) uniform PushConstants
{
	int uEntityID;
} constants;

layout(location = 0) out int fs_out;

void main()
{	
	
	fs_out = constants.uEntityID;

}