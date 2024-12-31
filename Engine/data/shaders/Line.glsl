#type vertex
#version 460  core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
};

layout(location = 0) out VertexOutput
{
	vec3 position;
	vec4 color;
} vs_out;

	
void main()
{
	gl_Position = u_projection * u_view * vec4(vPosition, 1.0);
	vs_out.position = vPosition;
	vs_out.color = vColor;
}

#type fragment
#version 460 core

layout(location = 0) in VertexOutput
{
	vec3 position;
	vec4 color;
} vs_in;

layout(location = 0) out vec4 fs_out;

void main()
{
	fs_out = vs_in.color;
}