#type compute
#version 460 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uTextureA; //scene
layout(set = 1, binding = 1) uniform sampler2D uTextureB; //bloom mip 0
layout(set = 1, binding = 2, rgba32f) uniform image2D uOutput;

layout(push_constant) uniform BloomSettings
{
    float uExposure;
    float uBloomStrength;
} pc;

vec3 FilamentBloomCombine(vec3 scene, vec3 bloom, float strength, float exposure)
{
    float bloomScale = strength * exposure;

    bloom = clamp(bloom * bloomScale, 0.0, 10.0);

    return scene + bloom;
}

void main()
{
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 sceneSize = imageSize(uOutput);

    vec2 uv = (vec2(dstCoord) + 0.5) / vec2(sceneSize);

    vec3 scene = texture(uTextureA, uv ).rgb;
    vec3 bloom = texture(uTextureB, uv ).rgb;

    float exposure = max(pc.uExposure, 0.0);
    float strength = max(pc.uBloomStrength, 0.0);

    vec3 combined = FilamentBloomCombine(scene, bloom, strength, exposure);

    imageStore(uOutput, dstCoord, vec4(combined, 1.0));
}