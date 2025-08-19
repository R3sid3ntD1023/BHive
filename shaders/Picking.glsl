#type vertex
#version 460

layout(location = 0) in vec3 vPosition;

layout(std140, binding = 0) uniform ObjectBuffer
{
	mat4 u_projection;
	mat4 u_view;
	mat4 u_model;
};

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(vPosition, 1);
}

#type fragment
#version 460


layout(location = 1) uniform int u_entity_id;

layout(location = 0) out vec3 fs_out;

void main()
{	
	
	fs_out = vec3(u_entity_id);

}