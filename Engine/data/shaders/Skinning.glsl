
#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

layout(std430, binding = 3) restrict readonly buffer Bones
{
	mat4 bones[MAX_BONES];
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

		
		bone_transform += bones[indices[i]] * weights[i];

	}

	return bone_transform;
}