#type vertex
#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPosition;

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

void main()
{
	bool instanced = gl_InstanceID != 0; 

	mat4 instance = (mat4(1) * (1.f - float(instanced))) +  (instances[gl_InstanceID] *  float(instanced));

	mat4 model =  o[gl_DrawID].WorldMatrix ;
	gl_Position = uProjection * uView *  instance * model* vec4(vPosition, 1);
}

#type fragment
#version 460 core

layout(location = 0) uniform vec3 uColor;
layout(location = 0) out vec4 fColor;

void main()
{
	fColor = vec4(uColor, 1);
}