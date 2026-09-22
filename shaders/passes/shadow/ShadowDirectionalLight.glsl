#type vertex
#version 460 core

#include <Shadow.vert>
#include <Constants.glsl>
#include <ShadowBuffer.glsl>


layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
	uint CascadeIndex;
} pc;


#define VIEW_PROJECTION DirShadowInfo[pc.LightIndex].Cascades[pc.CascadeIndex].ViewProjection

void main()
{
	#include <VertexCommon.glsl>
}

