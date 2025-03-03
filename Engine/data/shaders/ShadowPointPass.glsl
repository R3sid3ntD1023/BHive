#type vertex
#version 460 core 

#include <Skinning.glsl>
#include <Core.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

struct PerObjectData
{ 
	mat4 WorldMatrix;
};


layout(std430, binding = 1) restrict readonly buffer ObjectSSBO
{
	PerObjectData o[];
};

layout(std430, binding = 2) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};


void main()
{
	vec4 pos = vec4(vPosition , 1);
	mat4 boneTransform = Skinning(vWeights, vBoneIds);
	vec4 posL = boneTransform * pos;

	bool instanced = gl_InstanceID != -1; 

	mat4 instance = mix(mat4(1), instances[gl_InstanceID], float(instanced));

	mat4 model =  o[gl_DrawID].WorldMatrix * instance;
	gl_Position = model *  posL;
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
				gl_Position = uPointViewProjections[layer] * pos;
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

