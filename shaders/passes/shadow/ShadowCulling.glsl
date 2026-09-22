#type compute

#version 460 core

#include <Core.glsl>
#include <Intersections.glsl>
#include <Lighting.glsl>
#include <ShadowBuffer.glsl>

layout(local_size_x = 256) in;

layout(std430, set = 3, binding = 0) buffer ShadowObjects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = 3, binding = 1) buffer ShadowDraws
{
    IndirectDrawIndexedCommand drawCommands[];
};

layout(std430 , set = 3, binding = 2) buffer ShadowVisibility
{
    uint visibleCount;
    uint visibles[];
};


layout(push_constant) uniform PC
{
    uint LightType;
    uint LightIndex;
    uint LightFace;
} pc;

void main()
{
    uint id = gl_GlobalInvocationID.x;
    uint lightType = pc.LightType;

    if(id >= objectCount) return;

    ObjectData object = objects[id];

    bool visible = false;

    switch(lightType)
    {
        case 0:
        {
                uint index = pc.LightIndex;
                uint cascade = pc.LightFace;
                visible = SphereFrustumIntersection(
                    DirShadowInfo[index].Cascades[cascade].Frustum,
                    Sphere(object.center_radius.xyz, object.center_radius.w)
                );
        }
        break;
        case 1: //pointlights
        {
            uint index = pc.LightIndex;
            uint face = pc.LightFace;

            PointLight light = uPointLights[index];
            PointLightShadowInfo shadowInfo = PointShadowInfo[index];

            Sphere lightSphere = Sphere(light.Position.xyz, light.Position.w);
            Sphere objectSphere = Sphere(object.center_radius.xyz, object.center_radius.w);
    
            bool insideLightRange = SphereSphereIntersection(lightSphere, objectSphere);
            visible = insideLightRange && SphereFrustumIntersection(shadowInfo.Frustums[face], objectSphere);
        }
        break;
        case 2:
        {
                uint index = pc.LightIndex;
                visible = SphereFrustumIntersection(
                    SpotShadowInfo[index].Frustum,
                    Sphere(object.center_radius.xyz, object.center_radius.w)
                );
        }
        break;
    }

   

    if(visible)   
    {
        uint slot  = atomicAdd(visibleCount, 1);

        visibles[slot] = id;

        drawCommands[id].instanceCount = 1;
        drawCommands[id].firstInstance = slot;
    }
}