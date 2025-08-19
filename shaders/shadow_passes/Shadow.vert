mat4 boneTransform = GetBoneMatrix(vWeights, vBoneIds, bones);
vec4 worldPos = boneTransform * vec4(vPosition , 1);

bool instanced = gl_InstanceIndex != -1; 
mat4 instance = mix(mat4(1), instances[gl_InstanceIndex], float(instanced));
mat4 model =  object[gl_DrawID].WorldMatrix * instance;

gl_Position = model *  worldPos;