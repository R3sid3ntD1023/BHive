struct PerObjectData
{
	mat4 WorldMatrix;
	uint InstanceCount;
};

layout(std430, binding = 0) restrict readonly buffer ObjectSSBO
{
	PerObjectData object[];
};

layout(std430, binding = 1) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};

layout(std430, binding = 2) restrict readonly buffer Bones
{
	mat4 bones[MAX_BONES];
};