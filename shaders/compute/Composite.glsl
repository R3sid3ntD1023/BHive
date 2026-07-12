#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

vec3 FilamentBloomCombine(vec3 scene, vec3 bloom, float strength, float exposure)
{
    float bloomScale = strength * exposure;

    bloom = clamp(bloom * bloomScale, 0.0, 10.0);

    return scene + bloom;
}

layout (location = 0) in vec2 vUV;

layout(set = 1, binding = 0) uniform sampler2D uTextureA; //scene
layout(set = 1, binding = 1) uniform sampler2D uTextureB; //bloom mip 0

layout(push_constant) uniform BloomSettings
{
    float uExposure;
    float uBloomStrength;
} pc;

layout(location = 0) out vec4 oColor;

void main()
{
 
    vec3 scene = texture(uTextureA, vUV).rgb;
    vec3 bloom = texture(uTextureB, vUV).rgb;

    float exposure = max(pc.uExposure, 0.0);
    float strength = max(pc.uBloomStrength, 0.0);

    vec3 combined = FilamentBloomCombine(scene, bloom, strength, exposure);

    oColor = vec4(combined, 1.0);
}