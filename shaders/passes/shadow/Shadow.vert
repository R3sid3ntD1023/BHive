#include <Core.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std430, set = 3, binding = 0) readonly buffer Objects
{
    uint objectCount;
    ObjectData objects[];
};

layout(std430, set = 3, binding = 1) readonly buffer Draws
{
    IndirectDrawIndexedCommand drawCommands[];
};


layout(std430, set = 3, binding = 2) readonly buffer Visible
{
    uint visibleCount;
    uint visibleIndices[];
};

layout(std430, set = 3, binding = 3) readonly buffer Bones
{
	mat4 bones[];
};

mat4 GetBoneMatrix(const in ivec4 ids, const in vec4 weights, uint boneOffset);


#define MAX_BONE_INFLUENCE 4

bool HasBones(ivec4 indices)
{
	return indices.x != -1 || indices.y != -1 || indices.z != -1 || indices.w != -1;
}

mat4 GetBoneMatrix(const in ivec4 ids, const in vec4 weights, uint offset)
{
	if(!HasBones(ids)) return mat4(1.0f);

	mat4 bone = mat4(0.0f);

	for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if(ids[i] == -1)
		{
			continue;
		}

		bone += bones[offset + ids[i]] * weights[i];

	}

	return bone;
}