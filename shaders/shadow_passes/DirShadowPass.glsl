#type vertex
#version 460 core

#include <Core.glsl>
#include <Buffers.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;


void main()
{
	#include <Shadow.vert>
}

#type geometry
#version 460 core


#include <Core.glsl>
#include <Lighting.glsl>

layout(triangles, invocations = MAX_LIGHTS) in;
layout(triangle_strip, max_vertices = 3) out;



void main()
{

	if(gl_InvocationID < uNumShadowMaps.x)
	{
		for(int i = 0; i < 3; i++)
		{
			vec4 pos = gl_in[i].gl_Position;
			gl_Position = uDirViewProjections[gl_InvocationID] * pos;
			gl_Layer = gl_InvocationID;

			
			EmitVertex();
		}
		
		EndPrimitive();
	}
}
