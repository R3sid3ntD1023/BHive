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
	if(gl_InvocationID < uNumShadowMaps.z)
	{
		for(int i = 0; i < 3; i++)
		{
			gl_Position = uSpotViewProjections[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			
			EmitVertex();
		}
		
		EndPrimitive();
	}
}

#type fragment
#version 460 core


layout(location = 0) out vec4 FragColor;

void main()
{
	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

	FragColor = vec4(depth, moment2, 0, 1);
}