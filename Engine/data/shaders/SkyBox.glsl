#type vertex
#version 460 core

layout(location = 0) in vec3 vPosition;

layout(std140, binding = 0) uniform ObjectBuffer
{
	mat4 u_projection;
	mat4 u_view;
    vec2 u_near_far;
};

layout(location = 0) out struct vertex_output
{
	vec3 position;
} vs_out;


void main()
{
    vs_out.position =  vPosition;

    mat4 rotView = mat4(mat3(u_view));
    vec4 clipPos = u_projection * rotView * vec4(vPosition, 1);

    gl_Position = clipPos.xyww; 
}

#type fragment
#version 460 core

layout(location = 0) in struct vertex_output
{
	vec3 position;
} vs_in;

layout(binding = 0) uniform samplerCube environmentMap;

layout(location = 0) out vec4 fs_out;

void main()
{
    vec3 envColor = texture(environmentMap, vs_in.position).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    fs_out = vec4(envColor, 1.0);
}