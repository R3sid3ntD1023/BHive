

void ApplyLighting(vec3 geoPosition, vec3 geoNormal, vec3 geoViewDir, Material mat, inout ReflectedLight reflected)
{
#if defined( Direct )
	//Directional lights
	for(int i = 0; i < NumLights.x; i++)
	{
		DirectionalLight light = uDirectionalLights[i];
		IncidentLight directLight;

		GetDirectionalLightInfo(light, directLight);

		#if defined (DIRECTIONAL_SHADOW_MAPPING) 
		{		
			if (light.Color.a > 0.0)
			{
				DirectionalShadowInfo info = DirShadowInfo[i];
				float viewDepth = -(vs_in.View * vec4(geoPosition, 1.0)).z;
				float shadow = SampleShadowDepth(i, viewDepth, geoPosition, geoNormal, info, ShadowDirMaps);
				directLight.Color *= shadow;
				//directLight.Color *= DebugCascadeShadow(viewDepth, geoPosition, info);
			}
		}
		#endif


		Direct(geoPosition, geoNormal, geoViewDir, directLight, mat, reflected);
	}

	//pointlights
	for(int i = 0; i < NumLights.y; i++)
	{
		PointLight light = uPointLights[i];
		IncidentLight directLight;

		GetPointLightInfo(light, geoPosition, directLight);

		#if defined(POINT_SHADOW_MAPPING)
		{
			if (light.Color.a > 0.0)
			{
				PointLightShadowInfo shadow_info = PointShadowInfo[i];
				float shadow = GetPointShadow(i, geoPosition, normalize(directLight.Direction), directLight.Direction, shadow_info.ShadowNearFar.xy, ShadowPointMaps);
				float dist = length(directLight.Direction);
				float s = dist / light.Position.w;
				float shadowFade = 1.0 - smoothstep(0.8, 1.0, s);
				shadow = mix(1.0, shadow, shadowFade);
				directLight.Color *= shadow;
			}
		}
		#endif

		Direct(geoPosition, geoNormal, geoViewDir, directLight, mat, reflected);
	}

	//spotlights
	for(int i = 0; i < NumLights.z; i++)
	{
		SpotLight light = uSpotLights[i];
		IncidentLight directLight;

		GetSpotLightInfo(light, geoPosition, directLight);

		#if defined(SPOT_SHADOW_MAPPING)
		{
			if (light.Color.a > 0.0)
			{
				mat4 viewProj = SpotShadowInfo[i].ViewProjection;
				float shadow = GetSpotLightShadow(i, viewProj, geoPosition, ShadowSpotMaps);
				directLight.Color *= shadow;
			}
		}
		#endif

		Direct(geoPosition, geoNormal, geoViewDir, directLight, mat, reflected);
		
	}
#endif

#if defined(InDirect)
		vec3 irradiance = vec3(0.0);
	#endif
#if defined(InDirectSpecular)
		vec3 radiance = vec3(0.0);
#endif

#include <EnvironmentMaps.glsl>

#if defined(InDirect)
	InDirect(irradiance, geoPosition , geoNormal, geoViewDir, mat, reflected);
#endif

#if defined(InDirectSpecular)
	InDirectSpecular(radiance, geoPosition , geoNormal, geoViewDir, mat, reflected);
#endif
}