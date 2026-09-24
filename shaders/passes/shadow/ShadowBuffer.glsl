struct CascadeShadow
{
	mat4 ViewProjection;
	vec4 SplitData; // x= far plane
	Frustum Frustum;
};

struct DirectionalShadowInfo
{
	CascadeShadow Cascades[4];
	
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

int GetCascadeIndex(float viewDepth, CascadeShadow cascades[4])
{
	int layer = -1;
	for(int i = 0; i < 4; i++)
	{
		if(viewDepth <= cascades[i].SplitData.x)
		{
			layer = i;
			break;
		}
	}

	if(layer == -1)
	{
		layer = 3;
	}
	
	return layer;
}

vec4 GetDirectionalShadowUvs(int lightIndex, float viewDepth, vec3 geoPosition, DirectionalShadowInfo info)
{
	int cascade = GetCascadeIndex(viewDepth, info.Cascades);

	vec4 lightPos = info.Cascades[cascade].ViewProjection * vec4(geoPosition, 1.0);
	vec3 ndc = lightPos.xyz / lightPos.w;

	vec3 coord;
	coord.xy = ndc.xy * 0.5 + 0.5;
	coord.y = 1.0 - coord.y;
	coord.z = float(lightIndex * 4 + cascade);

	float depth = ndc.z;

	return vec4(coord, depth);
};


float SampleShadowDepth(int lightIndex, float viewDepth, vec3 geoPosition,DirectionalShadowInfo info, sampler2DArrayShadow shadowRaw)
{
	vec4 coord = GetDirectionalShadowUvs(lightIndex, viewDepth, geoPosition, info);
	vec3 uvw = coord.xyz;
	float depth = coord.w;

	if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
		return 1.0;
	
	float shadow = texture(shadowRaw, vec4(uvw, depth));
	
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