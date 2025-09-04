#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 2) in vec2 vTexCoord;
layout(location = 4) in int vTextureID;


layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec2 u_near_far;
    vec3 u_camera_position;
};

layout(location = 0) out flat int v_TextureID;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
	gl_Position = u_projection * u_view * vPosition;

	v_TextureID = vTextureID;
	v_TexCoord = vTexCoord;
}

#type fragment

#version 460 core


layout(location = 0) in flat int v_TextureID;
layout(location = 2) in vec2 v_TexCoord;


layout(binding = 0) uniform sampler2D uTextures[32];

layout(location = 0) out vec4 fs_out;

void main()
{
	float alpha = texture(uTextures[v_TextureID], v_TexCoord).a;
	fs_out = vec4(1.0, 0.0, 0.0, alpha) ;
}