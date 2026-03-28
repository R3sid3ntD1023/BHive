layout(location = 0) in vec3 vPosition;

layout(location = 0) out struct vertex_out
{
	vec3 position;
} vs_out;

layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
	vec4 u_camera_position;
};

void main()
{
	gl_Position = u_projection * u_view * vec4(vPosition, 1);
	vs_out.position = vPosition;
}