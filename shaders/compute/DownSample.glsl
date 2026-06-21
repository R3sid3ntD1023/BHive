#type compute
#version 460 core

#include <Sampling.glsl>

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 1, binding = 0 ) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, rgba32f) uniform image2D uOutput;

shared vec3 tile[16 + 4][16 +4]; //2-pixel halo on each side

void main() {

    ivec2 tileID = ivec2(gl_WorkGroupID.xy);
    ivec2 localID = ivec2(gl_LocalInvocationID.xy);

    ivec2 dstSize = imageSize(uOutput);
    ivec2 srcSize = textureSize(uSrcTexture, 0);

    vec2 scale = vec2(srcSize) / vec2(dstSize);

    //load shared memory tile(each thread loads muliple samples)
    for(int oy = localID.y; oy < 16 + 4; oy += 16)
    {
        for(int ox = localID.x; ox < 16 + 4; ox += 16)
        {
            ivec2 dstBase = tileID * 16 + ivec2(ox - 2 , oy - 2);
            dstBase = clamp(dstBase, ivec2(0), dstSize -  1);

            vec2 srcPos = (vec2(dstBase) + 0.5) * scale;
            ivec2 src = ivec2(clamp(srcPos, vec2(0.0), vec2(srcSize) - 1.0));

            vec2 uv = (vec2(src) + 0.5) / vec2(srcSize);
            tile[oy][ox] = texture(uSrcTexture, uv).rgb;
        }
    }

    barrier();

    ivec2 dstCoord = ivec2(tileID * 16 + localID);
    if(dstCoord.x >= dstSize.x || dstCoord.y >= dstSize.y)
        return;

    //now do the 13 tap downsample using shared memory
    vec3 sum = Downsample13TapShared(tile, localID);

    imageStore(uOutput, dstCoord, vec4(sum, 1.0));
}