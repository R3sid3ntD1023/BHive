mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);

bool instanced = (gl_InstanceIndex > 1); 
int index = max(gl_InstanceIndex - 1, 0);
mat4 instance = mix(  mat4(1), instances[index],  float(instanced));
mat4 model =   instance * object[gl_DrawID].WorldMatrix *bone_matrix;
vec4 worldPos = model * vec4(vPosition, 1);

mat3 normal_matrix = transpose(inverse(mat3(model)));
vec3 T = normalize(normal_matrix * vTangent);
vec3 N = normalize(normal_matrix * vNormal);
vec3 B = normalize(normal_matrix * vBiNormal);
	
gl_Position = u_projection * u_view * worldPos;