#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 2) in int vEntityID;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
    vec3 u_camera_position;
};


layout(location = 0) out flat int v_EntityID;

void main()
{
	gl_Position = u_projection * u_view * vPosition;

	v_EntityID = vEntityID;
}

#type fragment

#version 460 core

layout(location = 0) in flat int v_EntityID;

layout(location = 0) out int fs_out;

void main()
{

	fs_out = v_EntityID;
}