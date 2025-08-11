vec3 geometry_position = vs_in.Position;
vec3 geometry_normal = normal;
vec3 geometry_view_dir = normalize(vs_in.CameraPosition - vs_in.Position);

#if defined (USE_LIGHTING) && defined(Material)
	#if defined (Direct)
		#pragma unroll_loop_start
		for(int i = 0; i < uNumLights; i++)
		{
			Light light = uLights[i];
			uint type = light.type;
			IncidentLight direct_light;

			switch(type)
			{
				case 0:
				{
					GetDirectionLightInfo(light, direct_light);
					Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
					break;
				}
				case 1:
				{
					GetPointLightInfo(light, geometry_position, direct_light);
					Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
				
					break;
				}
				case 2:
				{
					GetSpotLightInfo(light, geometry_position, direct_light);
					Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
				
					break;
				}
				default:
					break;
			}
		}
		#pragma unroll_loop_end
	#endif
	#if defined(InDirect)
		vec3 irradiance = vec3(0.0);
	#endif
	#if defined(InDirectSpecular)
		vec3 radiance = vec3(0.0);
	#endif
#endif