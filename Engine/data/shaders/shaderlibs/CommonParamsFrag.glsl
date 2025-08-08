layout(location = 0) in struct VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec4 color;
	mat3 TBN;
} vs_in;

layout(std430, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
	vec3 u_camera_position;
};