#type compute
#version 460 core
#include <Core.glsl>

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0) uniform sampler2D u_Texture;   
layout(binding = 0 , r11f_g11f_b10f) uniform image2D uOutput;
   
layout(push_constant) uniform PushConstants
{
    vec4 u_FilterThreshold;
} constants;

vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve)
{
    // Pixel brightness
    float br = Max3(color.r, color.g, color.b);

    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

    return color;
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    float x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    float y = float(texelCoord.y)/(gl_NumWorkGroups.y);

    vec4 value = texture(u_Texture, vec2(x, y));
    vec4 color = QuadraticThreshold(value, constants.u_FilterThreshold.a, constants.u_FilterThreshold.rgb);
	
    imageStore(uOutput, texelCoord, color);
}