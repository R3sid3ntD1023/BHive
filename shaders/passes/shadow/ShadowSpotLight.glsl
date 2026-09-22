#type vertex
#version 460 core

#include <Constants.glsl>
#include <Shadow.vert>
#include <ShadowBuffer.glsl>

layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
} pc;

#define VIEW_PROJECTION SpotShadowInfo[pc.LightIndex].ViewProjection

void main()
{
	#include <VertexCommon.glsl>
}
