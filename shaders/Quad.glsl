#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vColor;
layout(location = 4) in int vTextureID;

layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
    vec3 u_camera_position;
};

layout(location = 0) out flat int v_TextureID;
layout(location = 1) out struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec4 color;
}
vs_out;

void main()
{
	gl_Position = u_projection * u_view * vPosition;

	vs_out.position = vPosition.xyz;
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;

	v_TextureID = vTextureID;
}

#type fragment

#version 460 core

#include <Core.glsl>

layout(location = 0) in flat int v_TextureID;
layout(location = 1) in struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec4 color;
}
vs_in;

layout(set = 1, binding = 0) uniform sampler2DArray uTexture;

layout(location = 0) out vec4 fs_out;

void main()
{
	vec4 color = texture(uTexture, vec3(vs_in.texcoord, float(v_TextureID)));

	fs_out = color * vs_in.color;
}