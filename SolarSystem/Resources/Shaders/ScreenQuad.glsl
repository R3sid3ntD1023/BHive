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

layout(location = 0) in struct VS_OUT{
	vec2 texCoord;
} vs_in;

//layout(binding = 0) uniform sampler2DMS uScreenTexture;
layout(binding  = 0) uniform sampler2D uColors;
layout(binding  = 1) uniform sampler2D uPositions;
layout(binding  = 2) uniform sampler2D uNormals;

//layout(location = 0) uniform ivec2 uViewportSize;

layout(location = 0) out vec4 fColor;

vec3 lightPos = vec3(0, 0, 0);

void main()
{
	//ivec2 size = uViewportSize;
	//size.x = int(size.x * vs_in.texCoord.x);
	//size.y = int(size.y * vs_in.texCoord.y);
	//vec4 sample1 = texelFetch(uScreenTexture, size, 0);
	//sample1 += texelFetch(uScreenTexture, size, 1);
	//sample1 += texelFetch(uScreenTexture, size, 2);
	//sample1 += texelFetch(uScreenTexture, size, 3);
	//sample1 /= 4.f;
	//fColor = sample1;

	vec3 position = texture(uPositions, vs_in.texCoord).rgb;
	vec3 normal = texture(uNormals, vs_in.texCoord).rgb;
	vec4 color = texture(uColors, vs_in.texCoord);

	float dist = length(lightPos - position);
	vec3 lightDir = normalize(lightPos - position);
	float ndotl = dot(normal, lightDir);
	float attenuation = 1.0 / (dist * dist);

	fColor = vec4(color.rgb * ndotl, 1);
}