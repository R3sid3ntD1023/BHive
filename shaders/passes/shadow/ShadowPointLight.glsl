#type vertex
#version 460 core 

#include <Constants.glsl>
#include <ShadowBuffer.glsl>
#include <Shadow.vert>

layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
	uint LightFace;
} pc;

#define VIEW_PROJECTION uPointShadowInfo[pc.LightIndex].ViewProjections[pc.LightFace]

void main()
{
	#include <VertexCommon.glsl>
}
