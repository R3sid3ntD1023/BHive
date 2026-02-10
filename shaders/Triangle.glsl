#type vertex

#version 460 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 uProjection;
    mat4 uView;
    vec2 uNearFar;
    vec4 uCameraPosition;
};

layout(location  = 0) out struct VERT_OUT 
{
	vec2 TexCoord;
} vs_out;

void main()
{
	vs_out.TexCoord = vTexCoord;
	gl_Position = uProjection * uView * vec4(vPosition, 1);
}


#type fragment

#version 460 core

layout(location  = 0) in struct VERT_OUT 
{
	vec2 TexCoord;
} vs_in;

//layout(binding = 1) uniform sampler2D u_Texture;
layout(location = 0) out vec4 f_out;

void main()
{
	//vec4 color = texture(u_Texture, vs_in.TexCoord);
	f_out = vec4(1,1,1,1);
}