float metalnessFactor = metalness;

#ifdef USE_METALNESS_MAP

	vec4 texelMetalness = texture(MetalnessMap, texCoord);

#ifdef IS_PACKED
	metalnessFactor *= texelMetalness.b;
#else
	metalnessFactor *= texelMetalness.r;
#endif
#endif