#ifdef USE_ENVIRONMENT_MAPS
	#if defined(InDirect)
		irradiance += GetIrradiance(geoNormal, IrradianceMap);
	#endif
	#if defined(InDirectSpecular)
	
		radiance += GetRadiance(geoViewDir, geoNormal,  mat.Roughness, mat.IrradianceF0, PreFilterMap, BRDFLutMap);
	#endif
#endif