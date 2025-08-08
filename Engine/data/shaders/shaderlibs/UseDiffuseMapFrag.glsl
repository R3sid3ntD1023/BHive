#if defined (USE_DIFFUSE_MAP)

bool is_diffuse_single_channel = (constants.u_Flags & DIFFUSE_SINGLE_CHANNEL) != 0;
diffuseColor *= texture(u_DiffuseMap, vs_in.texcoord);
diffuseColor.rgb = mix(diffuseColor.rgb, vec3(diffuseColor.r), float(is_diffuse_single_channel));
diffuseColor.rgb = pow(diffuseColor.rgb, vec3(2.2));

#endif