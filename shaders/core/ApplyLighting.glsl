
void ApplyLighting(vec3 geoPosition, vec3 geoNormal, vec3 geoViewDir, Material mat, inout ReflectedLight reflected)
{
#if defined( Direct )
	//Directional lights
	for(int i = 0; i < NumLights.x; i++)
	{
		DirectionalLight light = uDirectionalLights[i];
		IncidentLight directLight;

		GetDirectionalLightInfo(light, directLight);

		#if defined (USE_SHADOWMAPS)
			directLight.Color *= GetDirectionalShadow(i, geoPosition);
		#endif


		Direct(geoPosition, geoNormal, geoViewDir, directLight, mat, reflected);
	}

	//pointlights
	for(int i = 0; i < NumLights.y; i++)
	{
		PointLight light = uPointLights[i];
		IncidentLight directLight;

		GetPointLightInfo(light, geoPosition, directLight);

		#if defined (USE_SHADOWMAPS)
			directLight.Color *= GetPointShadow(i, geoPosition);
		#endif


		Direct(geoPosition, geoNormal, geoViewDir, directLight, mat, reflected);
	}

	//spotlights
	for(int i = 0; i < NumLights.z; i++)
	{
		SpotLight light = uSpotLights[i];
		IncidentLight directLight;

		GetSpotLightInfo(light, geoPosition, directLight);

		#if defined (USE_SHADOWMAPS)
			directLight.Color *= GetSpotLightShadow(i, geoPosition);
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