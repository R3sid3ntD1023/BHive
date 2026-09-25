#define SKINNING
#define MAX_BONE_INFLUENCE 4

layout(std430, set = SET_OBJECT, binding = 3) readonly buffer Bones
{
	mat4 bones[];
};

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