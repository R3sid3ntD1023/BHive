uint visibleIndex =  gl_BaseInstance;// + gl_InstanceIndex;
uint instanceID = visibleIndices[visibleIndex];
ObjectData object = objects[instanceID];

mat4 M = object.model;

#if defined(SKINNING)
    mat4 boneMatrix = GetBoneMatrix(vBoneIds, vWeights, object.boneOffset);
    M *= boneMatrix;
#endif

vec4 worldPos = M * vec4(vPosition, 1);

mat3 NM = transpose(inverse(mat3(M)));
vec3 T = normalize(NM * vTangent);
vec3 N = normalize(NM * vNormal);
vec3 B = normalize(NM * vBiNormal);

#ifdef APPLY_POSITION_OFFSET
	vec3 offset = ProcessVertex(worldPos.xyz, uCam.Time, uCam.Position.xyz);
	worldPos.xyz += offset;
#endif

gl_Position = VIEW_PROJECTION * worldPos;

