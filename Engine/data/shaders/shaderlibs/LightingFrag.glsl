vec3 geometry_position = vs_in.position;
vec3 geometry_normal = normal;
vec3 geometry_view_dir = u_camera_position;

#if defined (USE_LIGHTING) && defined (Direct) && defined(Material)
	#pragma unroll_loop_start
	for(int i = 0; i < uNumLights; i++)
	{
		Light light = uLights[i];
		uint type = light.type;

		switch(type)
		{
			case 1:
			{
				Direct(geometry_position, geometry_normal, geometry_view_dir, light, material, reflected_light);
				
				break;
			}
			default:
				break;
		}
	}
	#pragma unroll_loop_end
#endif