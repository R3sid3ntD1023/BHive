mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);
mat4 model = object[gl_DrawID].WorldMatrix * bone_matrix;

bool instanced = gl_InstanceIndex != -1; 
mat4 instance = mix(instances[gl_InstanceIndex], mat4(1), float(instanced));
vec4 worldPos = instance * model * vec4(vPosition , 1);

gl_Position = worldPos;