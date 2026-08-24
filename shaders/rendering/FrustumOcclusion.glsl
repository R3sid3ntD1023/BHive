#type compute

#version 460 core

layout(local_size_x = 256) in;

#include <Core.glsl>

layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec4 u_near_far;
	vec4 u_camera_position;
};

layout(std430, set = 3, binding = 0) buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = 3, binding = 1) buffer Draws
{
    IndirectDrawCommand drawCommands[];
};

layout(std430, set = 3, binding = 2) buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};


layout(push_constant) uniform CullingPC
{
  Frustum frustum;
} pc;

float GetSignedDistanceToPlane(vec4 plane, vec3 point)
{
    return dot(plane.xyz, point) - plane.w;
}

bool IsOnOrForwardPlane(vec4 plane, vec3 point, float radius)
{
    float distance = GetSignedDistanceToPlane(plane, point);
    return distance > -radius;
}

bool FrustumCullSphere(Frustum frustum, vec3 center, float radius)
{
    for(int i = 0; i < 6; i++)
    {
        if(!IsOnOrForwardPlane(frustum.planes[i], center, radius))
        {
            return false;
        }
    }
    return true;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;

    if(id >= objectCount) return;

    ObjectData object = objects[id];

    vec3 globalCenter = (u_view * object.model * vec4(object.center_radius.xyz, 1.0)).xyz;
    float maxScale = max(max(object.model[0][0], object.model[1][1]), object.model[2][2]);
    float scaledRadius = object.center_radius.w * maxScale;

    if(!FrustumCullSphere(pc.frustum, globalCenter, scaledRadius))
    {
        return;
    }

    //reserve slot atomically
    uint slot  = atomicAdd(visibleCount, 1);

    //write visible index
    visibleIndices[slot] = id;

    uint meshIndex = object.meshIndex;
    atomicAdd(drawCommands[meshIndex].instanceCount, 1);  
}