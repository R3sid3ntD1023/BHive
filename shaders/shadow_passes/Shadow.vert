mat4 bone_matrix = GetBoneMatrix(vWeights, vBoneIds, bones);

bool instanced = (gl_InstanceIndex > 1); 
int index = max(gl_InstanceIndex - 1, 0);
mat4 instance = mix(  mat4(1), instances[index],  float(instanced));
mat4 model =   instance * object[gl_DrawID].WorldMatrix *bone_matrix;
vec4 worldPos = model * vec4(vPosition, 1);

gl_Position = worldPos;