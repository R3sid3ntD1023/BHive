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
layout(triangle_strip, max_vertices = 18) out;

void main()
{
	if(gl_InvocationID < uNumShadowMaps.y)
	{
		for(int f = 0; f < 6; f++)
		{
			int layer = (gl_InvocationID * 6) + f;
			for(int i = 0; i < 3; i++)
			{
				vec4 pos = gl_in[i].gl_Position;
				gl_Position = uPointShadowInfo[gl_InvocationID].ViewProjections[f] * pos;
				gl_Layer = layer;
				
				EmitVertex();
			}
			
			EndPrimitive();
		}
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

