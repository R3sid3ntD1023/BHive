#type vertex

#version 460 core

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
	mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);
	mat4 model = object[gl_DrawID].WorldMatrix * bone_matrix;
	
	bool instanced = gl_InstanceIndex != -1; 
	mat4 instance = mix( instances[gl_InstanceIndex],  mat4(1), float(instanced));
	vec4 worldPos = instance * model * vec4(vPosition, 1);

	mat3 normal_matrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normal_matrix * vTangent);
	vec3 N = normalize(normal_matrix * vNormal);
	vec3 B = normalize(normal_matrix * vBiNormal);
	
	
	vs_out.Position = worldPos.xyz;
	vs_out.TBN = mat3(T, B, N);
	vs_out.Texcoord = vTexCoord;
	vs_out.Normal = N;
	vs_out.CameraPosition = u_camera_position;
	vs_out.Color = vColor;

	gl_Position *= u_projection * u_view * worldPos;
}

#type fragment

#version 460 core

#include <Core.glsl>

layout(push_constant) uniform PushConstants
{
	vec3 EmissiveColor;
} constants;

layout(location = 0) out vec4 fs_out;

void main()
{
	vec3 totalEmissiveRadiance = constants.EmissiveColor;

	fs_out = vec4(totalEmissiveRadiance, 1);
}
