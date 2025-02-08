#define BLINN_PHONG 1
#define LIGHTING_TYPE BLINN_PHONG


struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float brightness;
	float radius;
	float cutoff;
	float outercutoff;
	uint type;
};

#define MAX_LIGHTS 32
#define USE_POISSONDISK 1

layout(std140, binding = 1) uniform LightBuffer
{
	uint u_NumLights;
	Light lights[MAX_LIGHTS];	
};

layout(std140, binding = 2) uniform ShadowData
{
	uint u_NumDirectionalLights;
	mat4 u_ViewProjectionShadow[MAX_LIGHTS];
};

layout(std140, binding = 3) uniform SpotShadowData
{
	uint u_NumSpotLights;
	mat4 u_ViewProjectionSpotShadow[MAX_LIGHTS];
};

layout(std140, binding = 4) uniform PointShadowData
{
	uint u_NumPointLights;
	mat4 u_ViewProjectionPointShadow[MAX_LIGHTS * 6];
};


float SpotLightShadow(int light, vec3 position, in sampler2DArrayShadow shadow_array_texture);

float PointLightShadow(int light, vec3 position, vec3 lightDirection, vec3 lightDirectionUN,  vec2 near_far, 
	in samplerCubeArrayShadow point_shadow_array_texture);

float random(in vec3 seed, in float fFreq);

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

float PointLight(vec3 P, vec3 N, Light light)
{
	float dist = length(light.position - P);
	if(dist > light.radius) return 0;

	vec3 dir = light.position - P;
	float ndotl = dot(N, dir);
	float attenuation = max(1.0 - (dist * dist)/(light.radius*light.radius), 0.0);

	return ndotl * light.brightness * attenuation;
}

float DirLightShadow(int light, vec3 position, in sampler2DArrayShadow shadow_array_texture)
{
	vec4 fragLightPos = u_ViewProjectionShadow[light] * vec4(position, 1.0f);
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
	vec4 fragLightPos = u_ViewProjectionSpotShadow[light] * vec4(position, 1.0f);
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
}

float random(in vec3 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
	return fract(sin(dt) * 2105.2354);
}