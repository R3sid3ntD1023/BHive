#type vertex

#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(std140, binding = 0) uniform Matrices
{
	mat4 Projection;
	mat4 View;
	mat4 Model;
};

layout(location  = 0) out struct VERT_OUT 
{
	vec3 Color;
	vec2 TexCoord;
} vs_out;

void main()
{
	vs_out.Color = inColor; 
	vs_out.TexCoord = inTexCoord;
	gl_Position = Projection * View * Model * vec4(inPosition, 1);
}


#type fragment

#version 460 core

layout(location  = 0) in struct VERT_OUT 
{
	vec3 Color;
	vec2 TexCoord;
} vs_in;

layout(binding = 1) uniform sampler2D u_Texture;
layout(location = 0) out vec4 f_out;

void main()
{
	vec4 color = texture(u_Texture, vs_in.TexCoord);
	f_out = vec4(vs_in.Color, 1) * color;
}