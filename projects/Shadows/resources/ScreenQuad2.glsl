#type vertex

#version 460 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out struct VS_OUT
{
	vec2 texCoord;
} vs_out;

void main()
{
	gl_Position = vec4(vPosition, 1);
	vs_out.texCoord = vTexCoord;
}

#type fragment

#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in struct VS_OUT{
	vec2 texCoord;
} vs_in;

layout(bindless_sampler) uniform sampler2D uTexture;

layout(location = 0) out vec4 fColor;

void main()
{
	vec4 color = texture(uTexture, vs_in.texCoord);

	fColor = color;
}