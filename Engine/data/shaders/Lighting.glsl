
#define BLINN_PHONG 1
#define LIGHTING_TYPE BLINN_PHONG
#define MAX_LIGHTS 32
#define USE_POISSONDISK 1


struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float brightness;
	float radius;
	float innerCutoff;
	float outerCutoff;
	uint type;
};

layout(std430, binding = 1) uniform LightBuffer
{
	uint uNumLights;
	Light uLights[MAX_LIGHTS];
};

layout(std430, binding = 4) restrict readonly buffer ShadowSSBO
{
	uvec4 uNumShadowMaps;
	mat4 uDirViewProjections[MAX_LIGHTS];
	mat4 uPointViewProjections[MAX_LIGHTS * 6];
	mat4 uSpotViewProjections[MAX_LIGHTS];
};



const vec2 poissonDisk[9] = vec2[]
(
	vec2(-0.01529481f, -0.07395129f),
    vec2(-0.56232890f, -0.36484920f),
    vec2( 0.95519960f,  0.18418130f),
    vec2( 0.20716880f,  0.49262790f),
    vec2(-0.01290792f, -0.95755550f),
    vec2( 0.68047200f, -0.51716110f),
    vec2(-0.60139470f,  0.37665210f),
    vec2(-0.40243310f,  0.86631060f),
    vec2(-0.96646290f, -0.04688413f)
);

const vec3 v3poissonDisk[9] = vec3[](
    vec3(-0.023860920f, -0.115901396f,  0.985948205f),
    vec3(-0.649357200f, -0.542242587f,  0.066411376f),
    vec3( 0.956068397f,  0.285292149f, -0.865215898f),
    vec3( 0.228669465f,  0.698871136f,  0.355417848f),
    vec3(-0.001350721f, -0.997778296f, -0.866783142f),
    vec3( 0.602961421f, -0.725908756f, -0.338202178f),
    vec3(-0.672571659f,  0.557726085f, -0.027191758f),
    vec3(-0.123172671f,  0.978031158f, -0.663645744f),
    vec3(-0.995905936f, -0.073578961f, -0.894974828f)
);

const float light_size = 0.07;

float DirectionalLight(vec3 N, vec3 D)
{
	vec3 L = normalize(-D);
	float NdotL = max(dot(N, L), 0.0);
	return NdotL;
}

float PointLight(vec3 P, vec3 N, vec3 LP, float radius)
{
	float dist = length(LP - P);
	if(dist > radius) return 0;

	vec3 dir = LP - P;
	float ndotl = max(dot(N, dir), 0.0);

	//https://lisyarus.github.io/blog/posts/point-light-attenuation.html
	float s = dist / radius;
	if(s >= 1.0)
		return 0.0;
	float s2 = sqrt(s);

	float attenuation = sqrt(1 -s2) / (1 + radius * s);

	return ndotl * attenuation;
}

float SpotLight(vec3 P, vec3 N,  vec3 LP, vec3 LD, float radius, float outerCutoff, float innerCutoff)
{
	float pointlight =  PointLight(P, N, LP, radius);
	
	vec3 L = normalize(LP - P);
	
	float theta = dot(L, normalize(-LD ));
	float epsilon = innerCutoff - outerCutoff;
	float intensity = smoothstep(0, 1 , (theta - outerCutoff) / epsilon);
	
	return pointlight * intensity;
}

float DirLightShadow(int light, vec3 position, in sampler2DArrayShadow shadow_array_texture)
{
	vec4 fragLightPos = uDirViewProjections[light] * vec4(position, 1.0f);
	vec3 projCoords = fragLightPos.xyz / fragLightPos.w;
	projCoords = (projCoords + 1.f) * .5f;

#if USE_POISSONDISK
	float shadow = 0.0;
	int texel_size = textureSize(shadow_array_texture, 0).x;
	float shadow_region = light_size * projCoords.z;
	float shadow_size = shadow_region / 9;

	float angle = random(projCoords, 500.f) * (PI * 2.0f);
	vec2 rotate = vec2(sin(angle), cos(angle));

	for(int i = 0; i < 9; i++)
	{
		vec2 rotated_poisson = (poissonDisk[i].x * rotate.yx) + (poissonDisk[i].y * rotate * vec2(-1.0f, 1.0f));
		vec2 offset = rotated_poisson * shadow_size;
		vec3 uvc = projCoords + vec3(offset, 0.0f);
		
		shadow += texture(shadow_array_texture,vec4(uvc.xy, light, uvc.z));
	}

	return shadow / 9.0;
#else
	vec3 uvc = projCoords;
	return texture(shadow_array_texture,vec4(uvc.xy, light, uvc.z));
#endif

}

float SpotLightShadow(int light, vec3 position, in sampler2DArrayShadow shadow_array_texture)
{
	vec4 fragLightPos = uSpotViewProjections[light] * vec4(position, 1.0f);
	vec3 projCoords = fragLightPos.xyz / fragLightPos.w;
	projCoords = (projCoords + 1.f) * .5f;

#if USE_POISSONDISK
	float shadow = 0.0;
	int texel_size = textureSize(shadow_array_texture, 0).x;
	float shadow_region = light_size * projCoords.z;
	float shadow_size = shadow_region / 9;

	float angle = random(projCoords, 500.f) * (PI * 2.0f);
	vec2 rotate = vec2(sin(angle), cos(angle));

	for(int i = 0; i < 9; i++)
	{
		vec2 rotated_poisson = (poissonDisk[i].x * rotate.yx) + (poissonDisk[i].y * rotate * vec2(-1.0f, 1.0f));
		vec2 offset = rotated_poisson * shadow_size;
		vec3 uvc = projCoords + vec3(offset, 0.0f);
		
		shadow += texture(shadow_array_texture,vec4(uvc.xy, light, uvc.z));
	}

	return shadow / 9.0;

#else
	vec3 uvc = projCoords;
	return texture(shadow_array_texture,vec4(uvc.xy, light, uvc.z));
#endif
}

float PointLightShadow(int light, vec3 position, vec3 lightDirection, vec3 lightDirectionUN,  vec2 near_far, 
	in samplerCubeArrayShadow point_shadow_array_texture)
{
	vec3 absDirect = abs(lightDirectionUN);
	float dist = max(absDirect.x, max(absDirect.y, absDirect.z));
	float depth = (near_far.y + near_far.x) * dist;
	depth += (-2 * near_far.y * near_far.x);
	depth /= (near_far.y - near_far.x) * dist;
	depth = (depth * 0.5) + 0.5;

#if USE_POISSONDISK
	float shadow = 0.0;
	int texel_size = textureSize(point_shadow_array_texture, 0).x;
	float shadow_region = light_size * depth;
	float shadow_size = shadow_region / 9;
	float angle = random(position, 500.f) * (PI * 2.0f);
	vec3 rotate = vec3(sin(angle), cos(angle), 1);

	for(int i = 0; i < 9; i++)
	{
		vec3 rotated_poisson = (v3poissonDisk[i].x * rotate.yyx * rotate.zxx) + 
			(v3poissonDisk[i].y * rotate.xyx * rotate.zyy * vec3(-1.0f, 1.0f, 1.0f) +
			(v3poissonDisk[i].z * rotate.zxy * vec3(0.0f, -1.0f, 1.0f))
			);

		vec3 offset = rotated_poisson * shadow_size;
		vec3 uvc = -lightDirection + offset;
		shadow += texture(point_shadow_array_texture, vec4(uvc, light), depth);
	}

	return shadow / 9.0;

#else
	return texture(point_shadow_array_texture, vec4(-lightDirection, light), depth).r;
#endif
}


float SampleVariancePointLightShadow(int light, vec3 position, vec3 lightDirection, vec3 lightDirectionUN, vec2 near_far, 
	in samplerCubeArray point_shadow_array_texture)
{

	vec3 absDirect = abs(lightDirectionUN);
	float dist = max(absDirect.x, max(absDirect.y, absDirect.z));
	float depth = (near_far.y + near_far.x) * dist;
	depth += (-2 * near_far.y * near_far.x);
	depth /= (near_far.y - near_far.x) * dist;
	depth = (depth * 0.5) + 0.5;

	float shadow = 0.0;
	int texel_size = textureSize(point_shadow_array_texture, 0).x;
	float shadow_region = light_size * depth;
	float shadow_size = shadow_region / 9;
	float angle = random(position, 500.f) * (PI * 2.0f);
	vec3 rotate = vec3(sin(angle), cos(angle), 1);

	for(int i = 0; i < 9; i++)
	{	
		vec3 rotated_poisson = (v3poissonDisk[i].x * rotate.yyx * rotate.zxx) + 
			(v3poissonDisk[i].y * rotate.xyx * rotate.zyy * vec3(-1.0f, 1.0f, 1.0f) +
			(v3poissonDisk[i].z * rotate.zxy * vec3(0.0f, -1.0f, 1.0f))
			);
		vec3 offset =  rotated_poisson * shadow_size;
		vec3 uvc = -lightDirection + offset;
		vec2 moments =  texture(point_shadow_array_texture, vec4(uvc, light)).xy;
		shadow += SampleVariance(moments, depth);
	}

	return shadow / 9.0;
}

float SampleVarianceSpotLightShadow(int light, vec3 position, in sampler2DArray shadow_array_texture)
{
	vec4 fragLightPos = uSpotViewProjections[light] * vec4(position, 1.0f);
	vec3 projCoords = fragLightPos.xyz / fragLightPos.w;
	projCoords = (projCoords + 1.f) * .5f;

	vec3 uvc = projCoords;
	vec2 moments =  texture(shadow_array_texture,vec3(uvc.xy, light)).xy;

	return SampleVariance(moments, uvc.z);
}