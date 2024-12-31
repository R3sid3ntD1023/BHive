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

layout(std140, binding = 2) uniform ShadowData
{
	uint u_NumDirectionalLights;
	mat4 u_ViewProjectionShadow[MAX_LIGHTS];
};

layout(triangles, invocations = MAX_LIGHTS) in;
layout(triangle_strip, max_vertices = 3) out;

void main()
{
	if(gl_InvocationID < u_NumDirectionalLights)
	{
		for(int i = 0; i < 3; i++)
		{
			gl_Position = u_ViewProjectionShadow[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			
			EmitVertex();
		}
		
		EndPrimitive();
	}
}

#type fragment
#version 460 core

void main()
{

}