#type compute
#version 460 core

#include <Sampling.glsl>

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0 ) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, rgba32f) uniform image2D uOutput;

void main() {

    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dstSize = imageSize(uOutput);
  
    vec2 uv = (vec2(dstCoord) + 0.5) / vec2(dstSize);

    ivec2 srcSize = textureSize(uSrcTexture, 0);
    vec2 texel = 1.0 /  vec2(srcSize);

    vec3 sum = Downsample13Tap(uSrcTexture, uv, texel);

    imageStore(uOutput, dstCoord, vec4(sum, 1.0));
}