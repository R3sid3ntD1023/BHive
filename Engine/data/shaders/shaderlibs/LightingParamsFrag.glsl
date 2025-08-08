layout(push_constant) uniform PushConstants
{
	Material u_Material;
	uint u_Flags;
} constants;

#define DIFFUSE_SINGLE_CHANNEL 1 << 0
#define HAS_NORMAL_MAP 1 << 1
