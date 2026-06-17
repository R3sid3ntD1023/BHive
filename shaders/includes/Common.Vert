//mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);

uint objectIndex = gl_BaseInstance;
mat4 model =  object[gl_BaseInstance].Model;// * bone_matrix;
vec4 worldPos = model * vec4(vPosition, 1);

mat3 normal_matrix = transpose(inverse(mat3(model)));
vec3 T = normalize(normal_matrix * vTangent);
vec3 N = normalize(normal_matrix * vNormal);
vec3 B = normalize(normal_matrix * vBiNormal);
	
gl_Position = u_projection * u_view * worldPos;