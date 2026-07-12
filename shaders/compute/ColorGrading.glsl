#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

vec3 Grade(vec3 c, vec3 lift, vec3 gamma, vec3 gain, float saturation)
{
	c = c + lift;
	c = pow(max(c, 0.0), gamma);
	c = c * gain;

	float luma = dot(c, vec3(0.2126, 0.7152, 0.0722));
	c = mix(vec3(luma), c, saturation);

	return clamp(c, 0.0, 1.0);
}

layout(push_constant) uniform ColorGrading
{
	vec3 uLift;
	vec3 uGamma;
	vec3 uGain;
	float uSaturation;
} pc;

layout(set = 1, binding = 0) uniform sampler2D uTonemapped;
layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 oColor;

void main()
{
	vec3 color = texture(uTonemapped, vUV).rgb;

	vec3 lift = pc.uLift;
	vec3 gamma = pc.uGamma;
	vec3 gain = pc.uGain;
	float sat = pc.uSaturation;

	color = Grade(color, lift, gamma, gain, sat);

	oColor = vec4(color, 1.0);
}