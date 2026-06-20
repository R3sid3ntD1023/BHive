#type compute
#version 460 core

layout(local_size_x = 1, local_size_y = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uTonemapped;
layout(set = 1, binding = 1, rgba8) uniform image2D uOutput;

layout(push_constant) uniform ColorGrading
{
	vec3 uLift;
	vec3 uGamma;
	vec3 uGain;
	float uSaturation;
} pc;

vec3 Grade(vec3 c, vec3 lift, vec3 gamma, vec3 gain, float saturation)
{
	c = c + lift;
	c = pow(max(c, 0.0), gamma);
	c = c * gain;

	float luma = dot(c, vec3(0.2126, 0.7152, 0.0722));
	c = mix(vec3(luma), c, saturation);

	return clamp(c, 0.0, 1.0);
}

void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = (vec2(coord) + 0.5) / vec2(imageSize(uOutput));

	vec3 color = texture(uTonemapped, uv).rgb;

	vec3 lift = pc.uLift;
	vec3 gamma = pc.uGamma;
	vec3 gain = pc.uGain;
	float sat = pc.uSaturation;

	color = Grade(color, lift, gamma, gain, sat);

	imageStore(uOutput, coord, vec4(color, 1.0));
}