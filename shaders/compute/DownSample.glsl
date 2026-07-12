#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

#include <Sampling.glsl>

layout(location = 0) in vec2 vUV;

layout(set = 1, binding = 0 ) uniform sampler2D uSrcTexture;

layout(location = 0) out vec4 oColor;


void main() 
{
    //now do the 13 tap downsample using shared memory
    vec3 result = Downsample13Tap(uSrcTexture, vUV);

    oColor = vec4(result, 1.0);
}