#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec4 vColor;
layout(location = 3) in int vTextureID;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
};
		
layout(location = 0) out VertexOutput
{
	vec3 position;
	vec2 texcoord;
	vec4 color;
} vs_out;

layout (location = 3) out flat int v_TextureID;

void main()
{
	gl_Position = u_projection * u_view * vPosition;

	vs_out.position = vPosition.xyz;
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;

	v_TextureID = vTextureID;
}

#type fragment
#version 460 core

layout(location = 0) in VertexOutput
{
	vec3 position;
	vec2 texcoord;
	vec4 color;
} vs_in;

layout (location = 3) in flat int v_TextureID;

layout(binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 fs_out;

void main()
{
	vec4 color = texture(u_textures[v_TextureID], vs_in.texcoord);

	if(color.a < 1.0)
		discard;

	fs_out = color * vs_in.color;
	
}