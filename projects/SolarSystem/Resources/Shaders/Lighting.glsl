#type vertex

#version 460 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out struct VS_OUT
{
	vec2 texCoord;
} vs_out;

void main()
{
	gl_Position = vec4(vPosition, 1);
	vs_out.texCoord = vTexCoord;
}

#type fragment

#version 460 core

#include <Core.glsl>
#include <Lighting.glsl>


layout(location = 0) in struct VS_OUT{
	vec2 texCoord;
} vs_in;

layout(binding = 0) uniform sampler2D uColors;
layout(binding = 1) uniform sampler2D uPositions;
layout(binding = 2) uniform sampler2D uNormals;
layout(binding = 3) uniform sampler2D uEmission;

layout(location = 0) out vec4 fColor;

void main()
{
	vec3 position = texture(uPositions, vs_in.texCoord).rgb;
	vec3 normal = texture(uNormals, vs_in.texCoord).rgb;
	vec4 color = texture(uColors, vs_in.texCoord);
	vec3 emission = texture(uEmission, vs_in.texCoord).rgb;
	vec3 ambient = vec3(.1);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < uNumLights; i++)
	{
		Light light = uLights[i];
		uint type = light.type;

		switch(type)
		{
			case 1:
			{
				Lo += PointLight(position, normal, light.position, light.radius) * light.brightness * light.color;
				
				break;
			}
			default:
				break;
		}
	}

	color.rgb = (color.rgb * (Lo + ambient)) + emission ;

	fColor = color;
}