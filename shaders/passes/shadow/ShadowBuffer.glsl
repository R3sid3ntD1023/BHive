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

vec3 GetDirectionalShadowUvs(float viewDepth, vec3 geoPosition, DirectionalShadowInfo info, out int cascade)
{
	cascade = GetCascadeIndex(viewDepth, info.Cascades);

	vec4 lightPos = info.Cascades[cascade].ViewProjection * vec4(geoPosition, 1.0);
	vec3 uvw = lightPos.xyz / lightPos.w;
	uvw = uvw * 0.5 + 0.5;
	return uvw;
};

vec3 VisualizeShadowUvs(float viewDepth, vec3 geoPosition,DirectionalShadowInfo info)
{
	int cascade = 0;
	vec3 uvw = GetDirectionalShadowUvs(viewDepth, geoPosition, info, cascade);

	if(uvw.x < 0.0 || uvw.x > 1.0 || uvw.y < 0.0 || uvw.y > 1.0)
		return vec3(1, 0, 1);

	return vec3(uvw.xy, 0.0);
};

vec3 VisualizeShadowDepth(float viewDepth,vec3 geoPosition, DirectionalShadowInfo info)
{
	int cascade = 0;
	vec3 uvw = GetDirectionalShadowUvs(viewDepth, geoPosition, info, cascade);
		
	return vec3(uvw.z);
};

float SampleShadowDepth(int light, float viewDepth, vec3 geoPosition,DirectionalShadowInfo info, sampler2DArrayShadow shadowRaw)
{
	int cascade = 0;
	vec3 uvw = GetDirectionalShadowUvs(viewDepth, geoPosition, info, cascade);
	float shadow = texture(shadowRaw, vec4(uvw.xy, light * 4 + cascade, uvw.z));
	
	return shadow;
}

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