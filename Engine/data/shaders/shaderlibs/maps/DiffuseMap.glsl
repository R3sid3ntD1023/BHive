#if defined (USE_DIFFUSE_MAP)

diffuseColor *= texture(DiffuseMap, texCoord);
diffuseColor.rgb = pow(diffuseColor.rgb, vec3(2.2));

#endif