#ifdef USE_OPACITY_MAP
	diffuseColor.a *= texture(OpacityMap, texCoord).r;
#endif