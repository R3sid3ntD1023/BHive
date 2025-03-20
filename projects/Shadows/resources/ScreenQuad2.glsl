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

layout(location = 0) in struct VS_OUT{
	vec2 texCoord;
} vs_in;

layout(binding = 0) uniform sampler2D uTexture;

layout(location = 0) out vec4 fColor;

void main()
{
	vec4 color = texture(uTexture, vs_in.texCoord);

	fColor = color;
}