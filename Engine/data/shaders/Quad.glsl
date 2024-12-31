#type vertex
#version 460 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vColor;
layout(location = 4) in int vTextureID;
layout(location = 5) in int vFlags;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
};
		
layout (location = 0) out flat int v_TextureID;
layout (location = 1) out flat int v_Flags;
layout (location = 2) out flat int v_EntityID;
layout(location = 3) out VertexOutput
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec4 color;
	vec3 view;
} vs_out;



				
void main()
{
	gl_Position = u_projection * u_view * vPosition;

	vs_out.view = inverse(u_view)[3].xyz;
	vs_out.position = vPosition.xyz;
	vs_out.normal = vNormal;
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;

	v_TextureID = vTextureID;
	v_Flags = vFlags;
}

#type fragment
#version 460 core
#extension GL_ARB_shading_language_include : require

#include <Core.glsl>
#include <Lighting.glsl>

#define LIT BIT(0)

layout (location = 0) in flat int v_TextureID;
layout (location = 1) in flat int v_Flags;
layout (location = 2) in flat int v_EntityID;
layout(location = 3) in VertexOutput
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec4 color;
	vec3 view;
} vs_in;

layout(binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 fs_out;

vec3 CalculateDirectionalLight(vec3 norm, Light light);
vec3 CalculatePointLight(vec3 pos, vec3 norm,  Light light);

void main()
{

	vec4 color = texture(u_textures[v_TextureID], vs_in.texcoord);

	bool lit = (v_Flags & LIT) != 0;
	if(lit)
	{
		vec3 lo = vec3(0);

		for(int i = 0; i < u_NumLights; i++)
		{
			Light light = lights[i];
			switch(light.type)
			{
			case 0:
				lo += CalculateDirectionalLight(vs_in.normal, light); break;
			case 1:
				lo +=  CalculatePointLight(vs_in.position, vs_in.normal,  light); break;
			default:
				break;
			}
		}

		color.rgb *= lo;
	}

	if(color.a < .1)
		discard;

	fs_out = color;
	
}

vec3 CalculateDirectionalLight( vec3 norm, Light light)
{
	float ndotl = DirectionalLight(norm, light.direction);

	return ndotl * light.brightness * light.color;
}

vec3 CalculatePointLight(vec3 pos, vec3 norm,  Light light)
{
	float ndotl = PointLight(pos, norm, light);

	return ndotl * light.color * light.brightness;
}