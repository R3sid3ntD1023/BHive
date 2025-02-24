#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;

struct PerObjectData
{
	mat4 WorldMatrix;
};

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 uProjection;
	mat4 uView;
	mat4 u_model;
};
layout(std430, binding = 1) restrict readonly buffer InstanceBuffer
{
	mat4 matrices[];
};
layout(std430, binding = 2) restrict readonly buffer PerObjectSSBO
{
	PerObjectData object[];
};

layout(location = 0) uniform bool uInstanced = false;

void main()
{
	mat4 instance = mat4(1);
	if (uInstanced)
		instance = matrices[gl_InstanceID];

	vec4 worldPos = instance * object[gl_DrawID].WorldMatrix * vec4(vPos, 1);
	gl_Position = uProjection * uView * worldPos;
}

#type fragment

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 1) uniform bool uIsInFrustum;
layout(location = 2) uniform vec3 uColor;
layout(location = 0) out vec4 fColor;

void main()
{
	vec3 color = mix(vec3(1, 0, 0), uColor, uIsInFrustum);

	fColor = vec4(color, 1);
}
