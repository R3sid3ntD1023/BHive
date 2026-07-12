layout(location = 0) in vec3 vPosition;

layout(location = 0) out struct vertex_out
{
	vec3 position;
} vs_out;

layout(push_constant) uniform Camera
{
	mat4 u_ViewProjection;
};

void main()
{
	gl_Position = u_ViewProjection * vec4(vPosition, 1);
	vs_out.position = vPosition;
}