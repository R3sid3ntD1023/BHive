#type compute
#version 460 core

layout(local_size_x = 1, local_size_y = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, r11f_g11f_b10f) uniform image2D uOutput;

void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = (vec2(coord) + 0.5) / vec2(imageSize(uOutput));

	vec3 c = texture(uSrcTexture, uv).rgb;
	imageStore(uOutput, coord, vec4(c, 1.0));
}