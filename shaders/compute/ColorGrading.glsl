#type compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

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
	ivec2 dstCoord = ivec2(gl_WorkGroupID.xy * 16 + gl_LocalInvocationID.xy);

	ivec2 dstSize = imageSize(uOutput);

    if(dstCoord.x >= dstSize.x || dstCoord.y >= dstSize.y)
        return;

	vec2 uv = (vec2(dstCoord) + 0.5) / vec2(dstSize);

	vec3 color = texture(uTonemapped, uv).rgb;

	vec3 lift = pc.uLift;
	vec3 gamma = pc.uGamma;
	vec3 gain = pc.uGain;
	float sat = pc.uSaturation;

	color = Grade(color, lift, gamma, gain, sat);

	imageStore(uOutput, dstCoord, vec4(color, 1.0));
}