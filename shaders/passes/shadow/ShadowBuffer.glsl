#extension GL_EXT_texture_shadow_lod: enable

#define CASCADE_COUNT 5

struct CascadeShadow
{
	mat4 ViewProjection;
	vec4 SplitData; // x= far plane
	Frustum Frustum;
};

struct DirectionalShadowInfo
{
	vec4 Direction; //xyz = direction, w = farplane
	CascadeShadow Cascades[CASCADE_COUNT];
	
};

struct PointLightShadowInfo
{
	mat4 ViewProjections[6];
	vec4 ShadowNearFar;
	Frustum Frustums[6];
};

struct SpotLightShadowInfo
{
	mat4 ViewProjection;
	Frustum Frustum;
};

layout(std430, set = 0, binding = 5) restrict readonly buffer ShadowSSBO
{
	uvec4 NumShadowMaps;
	DirectionalShadowInfo DirShadowInfo[MAX_LIGHTS];
	PointLightShadowInfo PointShadowInfo[MAX_LIGHTS];
	SpotLightShadowInfo SpotShadowInfo[MAX_LIGHTS];
};

int GetCascadeIndex(float viewDepth, CascadeShadow cascades[CASCADE_COUNT])
{
	int layer = -1;
	for(int i = 0; i < CASCADE_COUNT; i++)
	{
		if(viewDepth < cascades[i].SplitData.x)
		{
			layer = i;
			break;
		}
	}

	if(layer == -1)
	{
		layer = CASCADE_COUNT;
	}
	
	return layer;
}

vec4 GetDirectionalShadowUvs(int lightIndex, float viewDepth, vec3 geoPosition,  DirectionalShadowInfo info)
{
	int cascade = GetCascadeIndex(viewDepth, info.Cascades);

	vec4 fragLightPos = info.Cascades[cascade].ViewProjection * vec4(geoPosition, 1.0);
	vec3 ndc = fragLightPos.xyz / fragLightPos.w;

	vec3 coord;
	coord.xy = ndc.xy * 0.5 + 0.5;
	coord.y = 1.0 - coord.y;
	coord.z = float(lightIndex * CASCADE_COUNT + cascade);

	float depth = ndc.z;

	return vec4(coord, depth);
};


float SampleShadowDepth(int lightIndex, float viewDepth, vec3 geoPosition, vec3 geoNormal, DirectionalShadowInfo info, sampler2DArrayShadow shadowRaw)
{
	vec4 coord = GetDirectionalShadowUvs(lightIndex, viewDepth, geoPosition, info);
	vec3 uvw = coord.xyz;
	float depth = coord.w;

	if(depth > 1.0)
	{
		return 0.0;
	}

	vec3 normal = geoNormal;
	vec3 lightDir  =info.Direction.xyz;
	float farPlane = info.Direction.w;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	if(coord.z == CASCADE_COUNT)
	{
		bias *= 1 / (farPlane * 0.5f);
	}
	else
	{
		bias *=1 / (info.Cascades[int(coord.z)].SplitData.x * 0.5f);
	}

	float shadow = texture(shadowRaw, vec4(uvw, depth), bias);
	
	return shadow;
}

vec3 VisualizeShadowUvs(int lightIndex, float viewDepth, vec3 geoPosition,DirectionalShadowInfo info)
{
	vec3 uvw = GetDirectionalShadowUvs(lightIndex, viewDepth, geoPosition, info).xyz;

	if(uvw.x < 0.0 || uvw.x > 1.0 || uvw.y < 0.0 || uvw.y > 1.0)
		return vec3(1, 0, 1);

	return vec3(uvw.xy, 0.0);
};


vec3 VisualizeShadowDepth(int lightIndex, float viewDepth,vec3 geoPosition, DirectionalShadowInfo info)
{
	vec3 uvw = GetDirectionalShadowUvs(lightIndex, viewDepth, geoPosition, info).xyz;
		
	return vec3(uvw.z);
};

vec3 DebugCascadeShadow(float viewDepth, vec3 geoPosition, DirectionalShadowInfo info)
{
	int cascade = GetCascadeIndex(viewDepth, info.Cascades);

	switch(cascade)
	{
		case 0: return vec3(1, 0, 0);
		case 1: return vec3(0, 1, 0);
		case 2: return vec3(0, 0, 1);
		default: return vec3(1, 1, 0);
	}
};