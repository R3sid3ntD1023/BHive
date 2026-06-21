#type compute
#version 460 core
#include <Core.glsl>

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uSceneColor;   
layout(set = 1, binding = 1 , rgba32f) uniform image2D uOutput;
   
layout(push_constant) uniform PushConstants
{
   float uThreshold;
} pc;

void main()
{
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(uOutput);

    vec2 uv = vec2(dstCoord) / vec2(size);

    float threshold = pc.uThreshold;
    vec3 color = texture(uSceneColor, uv).rgb;
    vec3 bloom = SoftKeeThreshold(color, threshold);

    imageStore(uOutput, dstCoord, vec4(bloom, 1.0));
}