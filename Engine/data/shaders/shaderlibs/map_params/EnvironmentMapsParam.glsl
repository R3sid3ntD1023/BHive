#ifdef USE_ENVIRONMENT_MAPS
layout(binding = 6) uniform samplerCube PreFilterMap;
layout(binding = 7) uniform samplerCube IrradianceMap;
layout(binding = 8) uniform sampler2D BRDFLUTMap;
#endif

vec3 GetIrradiance(const in vec3 normal)
{
	return texture(IrradianceMap, normal).rgb;
}

vec3 GetRadiance(const in vec3 viewDir, const in vec3 normal, const in float roughness, const in vec3 F0)
{
	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
	vec3 R = reflect(-viewDir, normal);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilerColor = textureLod(PreFilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(BRDFLUTMap, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	return prefilerColor * (F * envBRDF.x + envBRDF.y);
}