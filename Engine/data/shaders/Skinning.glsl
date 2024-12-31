
#define MAX_BONES 128
#define MAX_BONE_INFLUENCE 4

layout(std140, binding = 5) uniform BoneBuffer
{
	mat4 u_bone_matrices[MAX_BONES];
};

bool any_bone(ivec4 indices)
{
	return indices.x != -1 || indices.y != -1 || indices.z != -1 || indices.w != -1;
}

mat4 Skinning(vec4 weights, ivec4 indices)
{
	if(!any_bone(indices)) return mat4(1.0f);

	mat4 bone_transform = mat4(0.0f);

	for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if(indices[i] == -1)
		{
			continue;
		}

		if(indices[i] >= MAX_BONES)
		{
			break;
		}

		
		bone_transform += u_bone_matrices[indices[i]] * weights[i];

	}

	return bone_transform;
}