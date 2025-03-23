#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vColor;
layout(location = 4) in int vTextureID;
layout(location = 5) in int vFlags;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
};

layout(location = 0) out flat int v_TextureID;
layout(location = 1) out flat int v_Flags;
layout(location = 2) out VertexOutput
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec4 color;
	vec3 view;
}
vs_out;

void main()
{
	gl_Position = u_projection * u_view * vPosition;

	vs_out.view = inverse(u_view)[3].xyz;
	vs_out.position = vPosition.xyz;
	vs_out.normal = vNormal;
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;

	v_TextureID = vTextureID;
	v_Flags = vFlags;
}

#type fragment
#version 460 core

#include <Core.glsl>

#define LIT BIT(0)

layout(location = 0) in flat int v_TextureID;
layout(location = 1) in flat int v_Flags;
layout(location = 2) in VertexOutput
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec4 color;
	vec3 view;
}
vs_in;

layout(binding = 0) uniform sampler2D uTextures[32];

layout(location = 0) out vec4 fs_out;

void main()
{
	vec4 color = texture(uTextures[v_TextureID], vs_in.texcoord);

	fs_out = color * vs_in.color;
}