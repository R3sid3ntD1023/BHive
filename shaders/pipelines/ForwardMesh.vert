#include <Core.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

// @semantic Camera
layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
	mat4 u_projection;
	mat4 u_view;
	vec4 u_near_far;
	vec4 u_camera_position;
	Frustum frustum;
};

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


layout(location = 0) out struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
	vec3 DebugColor;
} vs_out;

mat4 GetBoneMatrix(const in ivec4 ids, const in vec4 weights, uint boneOffset);

void main()
{
	uint visibleIndex =  gl_BaseInstance;
	uint instanceID = visibleIndices[visibleIndex];
	ObjectData object = objects[instanceID];

	mat4 boneMatrix = GetBoneMatrix(vBoneIds, vWeights, object.boneOffset);
	mat4 model =  object.model * boneMatrix;
	vec4 worldPos = model * vec4(vPosition, 1);

	mat3 normal_matrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normal_matrix * vTangent);
	vec3 N = normalize(normal_matrix * vNormal);
	vec3 B = normalize(normal_matrix * vBiNormal);
		
	gl_Position = u_projection * u_view * worldPos;
	
	vs_out.Position = worldPos.xyz;
	vs_out.TBN = mat3(T, B, N);
	vs_out.Texcoord = vTexCoord;
	vs_out.Normal = N;
	vs_out.CameraPosition = u_camera_position.xyz;
	vs_out.Color = vColor;

	#if defined(SHOW_INSTANCE)
	vs_out.DebugColor = vec3(float(gl_InstanceIndex)/ 10.0, 0, 0);
	#else
	vs_out.DebugColor = vec3(float(gl_BaseInstance)/ 10.0, 0, 0);
	#endif
}

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