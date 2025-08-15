struct StandardMaterial
{
	vec3 Albedo;
	float Metalness;
	float Roughness;

	vec3 IrradianceF0;
};

#include <BDRFFunctions.glsl>

void Direct_Standard(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight light, const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 bdrf = CalculateBDRF(material.IrradianceF0, geoNormal, geoViewDir, light.Direction, material.Albedo, material.Metalness, material.Roughness);

	float NdotL = max(dot(geoNormal, light.Direction), 0.0);
	reflectedLight.DirectDiffuse += (bdrf * light.Color * NdotL);
}

void InDirect_Standard(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir,const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 F = fresnelSchlickRoughness(max(dot(geoNormal,geoViewDir), 0.0), material.IrradianceF0, material.Roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.Metalness;
	reflectedLight.IndirectDiffuse = kD *  irradiance * material.Albedo;
}

void InDirectSpecular_Standard(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir,const in StandardMaterial material, inout ReflectedLight reflectedLight)
{
	reflectedLight.IndirectSpecular =  irradiance;
}


#define Direct Direct_Standard
#define InDirect InDirect_Standard
#define InDirectSpecular InDirectSpecular_Standard
#define Material StandardMaterial