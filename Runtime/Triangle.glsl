#type vertex

#version 460 core

vec2 vPositions[3] = {vec2(0, -.5), vec2(.5, .5), vec2(-.5, .5)};
vec3 vColors[3] = {vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)};

layout(location  = 0) out struct VERT_OUT 
{
	vec3 Color;
} vs_out;

void main()
{
	vs_out.Color = vColors[gl_VertexIndex]; 
	gl_Position = vec4(vPositions[gl_VertexIndex], 0, 1);
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