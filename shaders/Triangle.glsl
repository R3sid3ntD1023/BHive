#type vertex

#version 460 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 uProjection;
    mat4 uView;
    vec2 uNearFar;
    vec4 uCameraPosition;
};

struct PerObjectData
{
	mat4 Model;
};

layout(std140, set = 0, binding = 1) restrict readonly buffer ObjectSSBO
{
	PerObjectData object[];
};

layout(location  = 0) out struct VERT_OUT 
{
	vec2 TexCoord;
} vs_out;

void main()
{
	vs_out.TexCoord = vTexCoord;

	mat4 model = object[gl_DrawID].Model;
	gl_Position = uProjection * uView * model * vec4(vPosition, 1);
}


#type fragment

#version 460 core

layout(location  = 0) in struct VERT_OUT 
{
	vec2 TexCoord;
} vs_in;

layout(set = 1, binding = 0) uniform sampler2D u_Texture;

layout(push_constant) uniform Constants
{
	vec3 u_Color;
	float u_Time;
};

layout(location = 0) out vec4 f_out;

void main()
{
	vec4 color = vec4(u_Color, 1) * texture(u_Texture, vs_in.TexCoord) ;
	color.rgb *= max(.5f * sin(u_Time) + .5f, .5);
	f_out = color;
}