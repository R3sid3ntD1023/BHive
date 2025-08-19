mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);
mat4 model = object[gl_DrawID].WorldMatrix * bone_matrix;
	
bool instanced = gl_InstanceIndex != -1; 
mat4 instance = mix( instances[gl_InstanceIndex],  mat4(1), float(instanced));
vec4 worldPos = instance * model * vec4(vPosition, 1);

mat3 normal_matrix = transpose(inverse(mat3(model)));
vec3 T = normalize(normal_matrix * vTangent);
vec3 N = normalize(normal_matrix * vNormal);
vec3 B = normalize(normal_matrix * vBiNormal);
	
gl_Position = u_projection * u_view * worldPos;