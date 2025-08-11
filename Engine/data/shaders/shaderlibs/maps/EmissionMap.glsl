#ifdef USE_EMISSION_MAP
	totalEmissiveRadiance *= texture(EmissionMap, texCoord).rgb;
#endif