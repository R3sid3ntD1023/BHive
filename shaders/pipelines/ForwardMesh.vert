#include <Core.glsl>
#include <Constants.glsl>
#include <Skinning.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;


layout(std140, set = SET_GLOBAL, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 View;
	vec4 NearFar;
	vec4 Position;
	Frustum Frustum;
} uCam;

layout(std430, set = SET_OBJECT, binding = 0) readonly buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = SET_OBJECT, binding = 1) readonly buffer Draws
{
    IndirectDrawIndexedCommand drawCommands[];
};


layout(std430, set = SET_OBJECT, binding = 2) readonly buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};

layout(location = 0) out struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
	vec3 DebugColor;
	mat4 View;
} vs_out;

#define VIEW_PROJECTION uCam.ViewProjection

void main()
{
	#include<VertexCommon.glsl>
	
	vs_out.Position = worldPos.xyz;
	vs_out.TBN = mat3(T, B, N);
	vs_out.Texcoord = vTexCoord;
	vs_out.Normal = N;
	vs_out.CameraPosition = uCam.Position.xyz;
	vs_out.Color = vColor;
	vs_out.View = uCam.View;
}

