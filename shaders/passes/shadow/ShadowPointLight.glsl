#type vertex
#version 460 core 


#include <Constants.glsl>
#include <Shadow.vert>
#include <ShadowBuffer.glsl>

layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
	uint LightFace;
} pc;

#define VIEW_PROJECTION PointShadowInfo[pc.LightIndex].ViewProjections[pc.LightFace]

void main()
{
	#include <VertexCommon.glsl>
}
