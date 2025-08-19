#if defined(InDirect) && defined(Material)
	InDirect(irradiance, geometry_position , geometry_normal, geometry_view_dir, material, reflected_light);
#endif

#if defined(InDirectSpecular) && defined(Material)
	InDirectSpecular(radiance, geometry_position , geometry_normal, geometry_view_dir, material, reflected_light);
#endif