struct LambertMaterial
{
	vec4 DiffuseColor;
	vec3 EmissionColor;	
};

void Direct_Lambert(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in Light light, const in LambertMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.DiffuseColor.rgb; 
	reflectedLight.DirectDiffuse += PointLight(geoPosition, geoNormal, light.position, light.radius) * light.brightness * light.color * diffuse;
}

#define Direct Direct_Lambert
#define Material LambertMaterial
#define USE_EMISSION