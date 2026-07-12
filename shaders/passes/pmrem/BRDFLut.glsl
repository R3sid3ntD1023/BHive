#type compute
#version 460 core

#include <PMREMFuncs.glsl>

layout(local_size_x =  8, local_size_y = 8, local_size_z = 1) in;

layout(rg16f, set = 1, binding =  0) uniform restrict writeonly image2D brdfLutTexture;

float GeomertySchlickGGX(float NDotV, float roughness)
{
	float r = roughness;
	float k = (r * r) / 2.0;
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

vec2 IntegratedBRDF(float NdotV, float roughness)
{
	vec3 V;
	V.x = sqrt(1.0 - NdotV * NdotV);
	V.y = 0.0;
	V.z = NdotV;

	float A = 0.0;
	float B = 0.0;

	vec3 N = vec3(0 , 0, 1);

	const uint SAMPLE_COUNT  = 1024u;
	for(uint i = 0 ; i  <SAMPLE_COUNT; i++)
	{
		vec2 Xi = Hamemersley(i , SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi, N, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(L.z, 0.0);
		float NdotH = max(H.z ,0.0);
		float VdotH = max(dot(V, H), 0.0);

		if(NdotL > 0.0)
		{
			float G = GeomertySmith(N, V, L, roughness);
			float G_Vis = (G * VdotH) / (NdotH * NdotV);
			float Fc = pow(1.0 - VdotH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);
	return vec2(A, B);

}

		

void main()
{
	vec2 texCoords = vec2(float(gl_GlobalInvocationID.x) , float(gl_GlobalInvocationID.y)) / float(512.0 - 1.0);

	vec2 integratedBRDF = IntegratedBRDF(max(texCoords.x, .0001), texCoords.y);

	imageStore(brdfLutTexture, ivec2(gl_GlobalInvocationID.xy), vec4(integratedBRDF, 0 , 0));
}
