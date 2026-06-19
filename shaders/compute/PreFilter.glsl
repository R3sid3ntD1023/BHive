#type compute
#version 460 core
#include <Core.glsl>

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uSceneColor;   
layout(set = 1, binding = 1 , rgba32f) uniform image2D uOutput;
   
layout(push_constant) uniform PushConstants
{
    vec4 uThreshold;
} pc;

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
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(uOutput);

    vec2 uv = vec2(dstCoord) / vec2(size);

    

    vec3 curve = pc.uThreshold.rgb;
    float threshold = pc.uThreshold.a;
    vec4 color = texture(uSceneColor, uv);
    vec4 bloom = QuadraticThreshold(color, threshold, curve);
    imageStore(uOutput, dstCoord, bloom);
}