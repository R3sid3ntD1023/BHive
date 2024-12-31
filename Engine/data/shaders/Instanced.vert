#version 460 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in int vEntityID;
layout(location = 7) in mat4 vModelMatrix;


layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
};

layout(location = 0) out flat int vs_entityid;
layout(location = 1) out struct vertex_output
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec4 color;
	vec3 view;
	mat3 TBN;
} vs_out;



void main()
{
	gl_Position = u_projection * u_view * vModelMatrix * vec4(vPosition, 1.0);

	vs_out.position = vec3(vModelMatrix * vec4(vPosition, 1.0));
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;
	vs_out.view = inverse(u_view)[3].xyz;

	vec3 T = mat3(transpose(inverse(vModelMatrix))) * vTangent;
	vec3 B = mat3(transpose(inverse(vModelMatrix))) * vBiNormal;
	vec3 N = mat3(transpose(inverse(vModelMatrix))) * vNormal;

	vs_out.normal = mat3(transpose(inverse(vModelMatrix))) * vNormal;
	vs_out.TBN = mat3(T, B, N);

	vs_entityid = vEntityID;
}