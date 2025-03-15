#type vertex

#version 460 core
#include <Core.glsl>

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

struct PerObjectData
{
	mat4 WorldMatrix;
};

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


layout(location = 0) out struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vs_out;

void main()
{

	bool instanced = gl_InstanceID != -1; 

	mat4 instance = mix( instances[gl_InstanceID],  mat4(1), float(instanced));

	mat4 model = object[gl_DrawID].WorldMatrix * instance;
	vec4 worldPos = model * vec4(vPos, 1);
	gl_Position = u_projection * u_view * worldPos;

	vs_out.position = worldPos.xyz;
	vs_out.texcoord = vTexCoord;
	mat3 normal_matrix = transpose(inverse(mat3(model)));
	vs_out.normal = normal_matrix * vNormal;
}

#type fragment

#version 460 core

#define SINGLE_CHANNEL 1 << 0

layout(location = 0) in struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vs_in;

layout(std430, binding = 3) uniform Material
{
	vec3 uColor;
	vec3 uEmission;
	uint uFlags;
	sampler2D uTexture;
};

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
	vec3 u_camera_position;
};

layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPosition;
layout(location = 2) out vec4 fNormal;
layout(location = 3) out vec4 fEmission;


float LinerizeDepth(float depth, float near, float far);


void main()
{
	uint is_single_channel = (uFlags & SINGLE_CHANNEL);

	vec3 color = pow(texture(uTexture, vs_in.texcoord).rgb, vec3(2.2));
	color.rgb = mix(color.rgb, vec3(color.r), is_single_channel);

	color += vec3(.1);
	color *= uColor;

	fColor = vec4(color, 1);
	fPosition = vec4(vs_in.position, 1);
	fNormal = vec4(normalize(vs_in.normal), 1);
	fEmission = vec4(uEmission, 1);

	gl_FragDepth = LinerizeDepth(gl_FragCoord.z, u_near_far.x, u_near_far.y);
}

float LinerizeDepth(float depth, float near, float far)
{
    float ndc = 2 * depth - 1;
    float eye = (2 * near * far) / ((far + near) - ndc * (far -near));
    float linearDepth = (eye - near) / (far - near);
    return linearDepth;
}
