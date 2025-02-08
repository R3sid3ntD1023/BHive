
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - min(cosTheta, 1.0), 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - min(cosTheta, 1.0), 0.0, 1.0), 5.0);
}


float DisributionGXX(vec3 N, vec3 H, float roughness)
{
	float a2 = roughness * roughness * roughness * roughness;
	float ndoth = max(dot(N, H), 0.0);
	float ndoth2 = ndoth * ndoth;

	float denom = (ndoth2 * (a2 - 1.0) + 1.0);
	return a2 /  (PI * denom * denom) + 0.0001;

}

float GeomertySchlickGGX(float NDotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	return NDotV / (NDotV * (1.0 - k) + k);
}

float GeomertySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float ndotv = max(dot(N, V), 0.0);
	float ndotl = max(dot(N, L), 0.0);
	float ggx2  = GeomertySchlickGGX(ndotv, roughness);
	float ggx1  = GeomertySchlickGGX(ndotl, roughness);

	return ggx1 * ggx2;
}


//N-Normal V-View Direction L-Light Direction
vec3 CalculateBDRF(vec3 F0, vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness)
{
	vec3 H = normalize(V + L);
	
	float NDF = DisributionGXX(N, H, roughness);
	float G = GeomertySmith(N, V, L, roughness);
	vec3 F =  fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 numerator = NDF *G * F;
	float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + .0001;
	vec3 specular = numerator / denom;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	float NDotL = max(dot(N, L), 0.0);

	return (kD * albedo / PI + specular) * NDotL;
}


vec3 CalculateDirectionalLightBDRF(vec3 F0, vec3 P, vec3 N, vec3 V, Light light, vec3 albedo, float metallic, float roughness)
{
	if(light.brightness <= 0.f) return vec3(0);

	vec3 L = normalize(-light.direction);
	vec3 BDRF = CalculateBDRF(F0, N, V, L, albedo, metallic, roughness);

	return BDRF  * light.color * max(0, light.brightness);
}

vec3 CalculateSpotLightBDRF(vec3 F0, vec3 P, vec3 N, vec3 V, Light light, vec3 albedo, float metallic, float roughness)
{
	if(light.brightness <= 0.f) return vec3(0);
	float pointlight =  PointLight(P, N, light);
	
	vec3 L = normalize(light.position - P);
	
	vec3 BDRF = CalculateBDRF(F0, N, V, L, albedo, metallic, roughness);

	float theta = dot(L, normalize(-light.direction ));
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = smoothstep(0, 1 , (theta - light.outercutoff) / epsilon);
	
	return BDRF  * light.color * max(0, light.brightness) * pointlight * intensity;
}

vec3 CalculatePointLightBDRF(vec3 F0, vec3 P, vec3 N, vec3 V, Light light, vec3 albedo, float metallic, float roughness)
{
	if(light.brightness <= 0.f) return vec3(0);

	vec3 L = light.position - P;
	float dist = length(light.position - P);

	float pointlight =  PointLight(P, N, light);
	vec3 BDRF = CalculateBDRF(F0, N, V, normalize(L), albedo, metallic, roughness);

	return BDRF  * light.color * max(0, light.brightness) * pointlight;
}

vec3 EnvironmentLighting(vec3 F0, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, in samplerCube irradiance,
	in samplerCube prefilter,
	in sampler2D brdflut)
{
	

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 diffuse = texture(irradiance, N).rgb * albedo;

	//specular reflectance
	vec3 R = reflect(-V, N);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilerColor = textureLod(prefilter, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(brdflut, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilerColor * (F * envBRDF.x + envBRDF.y);

	return (kD * diffuse + specular);
}