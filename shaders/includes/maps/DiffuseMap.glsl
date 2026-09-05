#if defined (USE_DIFFUSE_MAP)

vec4 color = texture(DiffuseMap, texCoord);
color.rgb = pow(color.rgb, vec3(2.2));

diffuseColor.rgb *= color.rgb; 
diffuseColor.a *= color.a;
#endif