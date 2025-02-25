#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

struct PerObjectData
{
	mat4 WorldMatrix;
	uint Flags;
};

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
};
layout(std430, binding = 1) restrict readonly buffer InstanceBufferSSBO
{
	mat4 matrices[];
};
layout(std430, binding = 2) restrict readonly buffer PerObjectSSBO
{
	PerObjectData object[];
};

layout(location = 0) out struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vs_out;

#define INSTANCED_FLAG 0x01

void main()
{
	uint flags = object[gl_DrawID].Flags;
	bool instanced = (flags & INSTANCED_FLAG) != 0; 

	mat4 instance = (mat4(1) * (1.f - float(instanced))) +  (matrices[gl_InstanceID] *  float(instanced));

	mat4 model = object[gl_DrawID].WorldMatrix;
	vec4 worldPos = instance * model * vec4(vPos, 1);
	vs_out.position = worldPos.xyz;
	vs_out.texcoord = vTexCoord;

	mat3 normal_matrix = transpose(inverse(mat3(model)));
	vs_out.normal = normal_matrix * vNormal;

	gl_Position = u_projection * u_view * worldPos;
}

#type fragment

#version 460 core
#extension GL_NV_bindless_texture : require
#extension GL_NV_uniform_buffer_std430_layout : require

#define SINGLE_CHANNEL 1 << 0

layout(location = 0) in struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vs_in;

//layout(bindless_sampler) uniform sampler2D uTexture;

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
