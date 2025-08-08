
bool instanced = gl_InstanceIndex != -1; 
mat4 instance = mix( instances[gl_InstanceIndex],  mat4(1), float(instanced));
mat4 model = object[gl_DrawID].WorldMatrix * instance;
vec4 worldPos = model * vec4(vPosition, 1);
gl_Position = u_projection * u_view * worldPos;

vs_out.position = worldPos.xyz;
vs_out.texcoord = vTexCoord;
mat3 normal_matrix = transpose(inverse(mat3(model)));
vec3 T = normalize(normal_matrix * vTangent);
vec3 N = normalize(normal_matrix * vNormal);
vec3 B = normalize(normal_matrix * vBiNormal);

vs_out.normal = N;
vs_out.TBN = mat3(T, B, N);