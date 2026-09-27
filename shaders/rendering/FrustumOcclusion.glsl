#type compute

#version 460 core

#include <Constants.glsl>

layout(local_size_x = 256) in;

#include <Core.glsl>
#include <Intersections.glsl>

layout(std140, set = SET_GLOBAL, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
    mat4 View;
	vec4 NearFar;
	vec4 Position;
    Frustum Frustum;
} uCam;

layout(std430, set = SET_OBJECT, binding = 0) buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = SET_OBJECT, binding = 1) buffer Draws
{
    IndirectDrawIndexedCommand drawCommands[];
};

layout(std430, set = SET_OBJECT, binding = 2) buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};

void main()
{
    uint id = gl_GlobalInvocationID.x;

    if(id >= objectCount) return;

    ObjectData object = objects[id];

    Sphere s = Sphere(object.center_radius.xyz,object.center_radius.w);

    bool visible = SphereFrustumIntersection(uCam.Frustum, s);

    drawCommands[id].instanceCount = 0;
    drawCommands[id].firstInstance = 0;
    
    if(visible)   
    {
        uint slot  = atomicAdd(visibleCount, 1);

        visibleIndices[slot] = id;

        drawCommands[id].instanceCount = 1;
        drawCommands[id].firstInstance = slot;
    }
    
}