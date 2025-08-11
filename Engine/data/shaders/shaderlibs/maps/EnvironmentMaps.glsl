#ifdef USE_ENVIRONMENT_MAPS
	#if defined(InDirect)
		irradiance += GetIrradiance(geometry_normal);
	#endif
	#if defined(InDirectSpecular)
	
		radiance += GetRadiance(geometry_view_dir, geometry_normal,  material.Roughness,material.IrradianceF0);
	#endif
#endif