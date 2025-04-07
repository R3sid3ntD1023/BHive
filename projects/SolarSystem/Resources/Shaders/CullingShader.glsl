#type vertex

#version 460 core

#include <Core.glsl>

layout(location = 0) in vec3 vPos;

struct PerObjectData
{
	mat4 WorldMatrix;
};

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 uProjection;
	mat4 uView;
	vec2 uNearFar;
	vec3 uCameraPosition;
};

layout(std430, binding = 1) restrict readonly buffer PerObjectSSBO
{
	PerObjectData object[];
};

layout(std430, binding = 2) restrict readonly buffer InstanceBuffer
{
	mat4 matrices[];
};

void main()
{
	bool instanced = gl_InstanceIndex != -1;

	mat4 instance = mix(mat4(1), matrices[gl_InstanceIndex] , float(instanced));

	vec4 worldPos = instance * object[gl_DrawID].WorldMatrix * vec4(vPos, 1);
	gl_Position = uProjection * uView * worldPos;
}

#type fragment

#version 460 core

layout(std430 , binding = 0) uniform Data
{
	bool uIsInFrustum;
	vec3 uColor;
};

layout(location = 0) out vec4 fColor;

void main()
{
	vec3 color = mix(vec3(1, 0, 0), uColor, float(uIsInFrustum));

	fColor = vec4(color, 1);
}
