#type compute

#version 460 core

layout(local_size_x = 256) in;

#include <Core.glsl>

layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
	vec4 nearFar;
	vec4 cameraPosition;
    Frustum frustum;
} u_cam;

layout(std430, set = 3, binding = 0) buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = 3, binding = 1) buffer Draws
{
    IndirectDrawIndexedCommand drawCommands[];
};

layout(std430, set = 3, binding = 2) buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};

bool SphereIntersection(Frustum frustum, vec3 c, float r)
{
    for(int i = 0; i < 6; i++)
    {
        vec4 plane = frustum.planes[i];
        vec3 n = plane.xyz;
        float d = plane.w;
        if((dot(c, n) + d + r) >= 0.0)
            return false;
    }

    return true;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;

    if(id >= objectCount) return;

    ObjectData object = objects[id];

    vec3 center = object.center_radius.xyz;
    float radius = object.center_radius.w;
    
    bool visible = SphereIntersection(u_cam.frustum, center, radius);
    
    objects[id].debugcolor.xyz = vec3(float(id)/ 10.f, 0.0, 1.0 - float(id) / 10.0);
    objects[id].debugcolor.xyz = visible ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);

    //reserve slot atomically
    uint slot  = atomicAdd(visibleCount, 1);

    if(!visible)
        return;
 
    //write visible index
    visibleIndices[slot] = id;

    uint count = atomicAdd(drawCommands[id].instanceCount, 1);
    drawCommands[id].firstInstance = id;
}