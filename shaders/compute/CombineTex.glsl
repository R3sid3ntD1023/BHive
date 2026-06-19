#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0) uniform sampler2D uTextureA; //scene
layout(set = 1, binding = 1) uniform sampler2D uTextureB; //bloom mip 0
layout(set = 1, binding = 2, rgba32f) uniform image2D uOutput;

void main()
{
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(uOutput);

    vec2 uv = vec2(dstCoord) / vec2(size);

    vec4 scene = texture(uTextureA, uv );
    vec4 bloom = texture(uTextureB, uv );

    vec4 color = scene + bloom;
    imageStore(uOutput, dstCoord, color);
}