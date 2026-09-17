uint visibleIndex =  gl_BaseInstance;
uint instanceID = visibleIndices[visibleIndex];
ObjectData object = objects[instanceID];

#if defined(SKINNING)
    mat4 boneMatrix = GetBoneMatrix(vBoneIds, vWeights, object.boneOffset);
    mat4 M =  object.model * boneMatrix;
#else
    mat4 M = object.model;
#endif

vec4 worldPos = M * vec4(vPosition, 1);

mat3 NM = transpose(inverse(mat3(M)));
vec3 T = normalize(NM * vTangent);
vec3 N = normalize(NM * vNormal);
vec3 B = normalize(NM * vBiNormal);
    
gl_Position = VIEW_PROJECTION * worldPos;