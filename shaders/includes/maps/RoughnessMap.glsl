#ifdef USE_ROUGHNESS_MAP

	vec4 texelRoughness = texture(RoughnessMap, texCoord);

#ifdef IS_PACKED
	//read channel g
	roughnessFactor *= texelRoughness.g;
#else
	roughnessFactor *= texelRoughness.r;
#endif
#endif