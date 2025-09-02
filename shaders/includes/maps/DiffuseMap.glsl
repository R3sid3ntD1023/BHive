#if defined (USE_DIFFUSE_MAP)

diffuseColor.rgb *= texture(DiffuseMap, texCoord).rgb;
diffuseColor.rgb = pow(diffuseColor.rgb, vec3(2.2));

#endif