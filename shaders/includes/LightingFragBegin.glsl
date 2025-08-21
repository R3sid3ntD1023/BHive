vec3 geometry_position = vs_in.Position;
vec3 geometry_normal = normal;
vec3 geometry_view_dir = normalize(vs_in.CameraPosition - vs_in.Position);

#if defined (USE_LIGHTING) && defined(Material)
	#if defined (Direct)
		#pragma unroll_loop_start

		for(int i = 0; i < uNumDirLights; i++)
		{
			DirectionalLight light = uDirectionalLights[i];
			IncidentLight direct_light;
			GetDirectionLightInfo(uDirectionalLights[i], direct_light);
			Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
		}
		#pragma unroll_loop_end

		#pragma unroll_loop_start
		for(int i = 0; i < uNumPointLights; i++)
		{
				PointLight light = uPointLights[i];
				IncidentLight direct_light;

				GetPointLightInfo(light, geometry_position, direct_light);

				#if defined(USE_SHADOWMAPS)
					PointLightShadow pointLightShadow = uPointShadows[i];
					direct_light.Color *= GetPointShadow(i, geometry_position, normalize(direct_light.Direction), direct_light.Direction, pointLightShadow.ShadowNearFar, u_shadow_point_map);
				#endif

				Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
			
		}
		#pragma unroll_loop_end

		#pragma unroll_loop_start
		for(int i = 0; i < uNumSpotLights; i++)
		{
			SpotLight light = uSpotLights[i];
			IncidentLight direct_light;
			GetSpotLightInfo(light, geometry_position, direct_light);

			#if defined(USE_SHADOWMAPS)
				direct_light.Color *= GetSpotLightShadow(i, geometry_position, u_shadow_spot_map);
			#endif
			Direct(geometry_position, geometry_normal, geometry_view_dir, direct_light, material, reflected_light);
				
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