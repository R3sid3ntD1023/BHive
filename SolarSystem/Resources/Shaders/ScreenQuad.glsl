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

layout(location = 0) in struct VS_OUT{
	vec2 texCoord;
} vs_in;

layout(bindless_sampler) uniform sampler2D uHDR;
layout(bindless_sampler) uniform sampler2D uBloom;

layout(location = 0) out vec4 fColor;

void main()
{
	vec4 hdr = texture(uHDR, vs_in.texCoord);
	vec4 bloom = texture(uBloom, vs_in.texCoord);

	vec4 color = mix(hdr, hdr + bloom, 1.f);

	fColor = color;
}