struct PerObjectData
{
	mat4 Model;
	//uint InstanceCount;
};

layout(std140, set = 3, binding = 0) restrict readonly buffer ObjectSSBO
{
	PerObjectData object[];
};


//layout(std140, set = 3, binding = 1) restrict readonly buffer InstanceSSBO
//{
//	mat4 instances[];
//};
//
//layout(std140, set = 3, binding = 2) restrict readonly buffer Bones
//{
//	mat4 bones[MAX_BONES];
//};