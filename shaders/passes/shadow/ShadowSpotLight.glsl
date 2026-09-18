#type vertex
#version 460 core

#include <Constants.glsl>
#include <ShadowBuffer.glsl>
#include <Shadow.vert>

layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
} pc;

#define VIEW_PROJECTION uSpotViewProjections[pc.LightIndex]

void main()
{
	#include <VertexCommon.glsl>
}
