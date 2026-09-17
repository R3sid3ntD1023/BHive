#type vertex
#version 460 core 

#include <Constants.glsl>
#include <ShadowBuffer.glsl>
#include <Shadow.vert>

layout(push_constant) uniform ShadowPC
{
	uint LightIndex;
	uint LightFace;
} pc;

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
		
	gl_Position = uPointShadowInfo[pc.LightIndex].ViewProjections[pc.LightFace] * worldPos;
}

#type fragment
#version 460 core 

//layout(location = 0) out vec4 FragColor;

void main()
{
	// float depth = gl_FragCoord.z;
	// float dx = dFdx(depth);
	// float dy = dFdy(depth);
	// float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

	// FragColor = vec4(depth, moment2, 0, 1);
}

