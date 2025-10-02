#type vertex

#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(std140, binding = 0) uniform Matrices
{
	mat4 Projection;
	mat4 View;
	mat4 Model;
};

layout(location  = 0) out struct VERT_OUT 
{
	vec3 Color;
} vs_out;

void main()
{
	vs_out.Color = inColor; 
	gl_Position = Projection * View * Model * vec4(inPosition, 1);
}


#type fragment

#version 460 core

layout(location  = 0) in struct VERT_OUT 
{
	vec3 Color;
} vs_in;

layout(location = 0) out vec4 f_out;

void main()
{
	f_out = vec4(vs_in.Color, 1);
}