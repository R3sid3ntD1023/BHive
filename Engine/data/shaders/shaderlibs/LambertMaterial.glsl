struct LambertMaterial
{
	vec4 DiffuseColor;
};

void Direct_Lambert(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight directLight, const in LambertMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.DiffuseColor.rgb; 
	vec3 irradiance = max(dot(geoNormal, directLight.Direction), 0.0) * diffuse;
	reflectedLight.DirectDiffuse += irradiance * directLight.Color;
}

#define Direct Direct_Lambert
#define Material LambertMaterial
#define USE_EMISSION