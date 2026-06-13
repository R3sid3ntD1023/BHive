//mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);

bool instanced = false; //object_data.InstanceCount > 0; 
//mat4 instance = mix(  mat4(1), instances[gl_InstanceIndex],  float(instanced));

uint objectIndex = gl_BaseInstance + gl_InstanceIndex;
mat4 model =  object[gl_BaseInstance].Model;// instance * object_data.WorldMatrix *bone_matrix;
vec4 worldPos = model * vec4(vPosition, 1);

mat3 normal_matrix = transpose(inverse(mat3(model)));
vec3 T = normalize(normal_matrix * vTangent);
vec3 N = normalize(normal_matrix * vNormal);
vec3 B = normalize(normal_matrix * vBiNormal);
	
gl_Position = u_projection * u_view * worldPos;