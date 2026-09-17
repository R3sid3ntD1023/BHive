#include <Core.glsl>
#include <Skinning.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std430, set = 3, binding = 0) readonly buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = 3, binding = 1) readonly buffer Draws
{
    IndirectDrawIndexedCommand drawCommands[];
};


layout(std430, set = 3, binding = 2) readonly buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};
