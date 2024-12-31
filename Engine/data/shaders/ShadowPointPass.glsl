#type vertex
#version 460 core 
#extension GL_ARB_shading_language_include : require

#include <Skinning.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std140, binding = 0) uniform ObjectBuffer
{
	mat4 u_projection;
	mat4 u_view;
	mat4 u_model;
};


void main()
{
	mat4 bone_transform = Skinning(vWeights, vBoneIds);
	gl_Position = u_model * bone_transform * vec4(vPosition, 1);
}

#type geometry
#version 460 core

#define MAX_LIGHTS 32

layout(std140, binding = 4) uniform PointShadowData
{
	uint u_NumPointLights;
	mat4 u_ViewProjectionPointShadow[MAX_LIGHTS * 6];
};

layout(triangles, invocations = MAX_LIGHTS) in;
layout(triangle_strip, max_vertices = 18) out;


void main()
{
	if(gl_InvocationID < u_NumPointLights)
	{
		for(int f = 0; f < 6; f++)
		{
			int layer = (gl_InvocationID * 6) + f;
			for(int i = 0; i < 3; i++)
			{
				gl_Position = u_ViewProjectionPointShadow[layer] * gl_in[i].gl_Position;
				gl_Layer = layer;
				
				EmitVertex();
			}
			
			EndPrimitive();
		}
	}
}

#type fragment
#version 460 core 

void main()
{

}