struct PointLightShadowInfo
{
	mat4 ViewProjections[6];
	vec2 ShadowNearFar;
};

layout(std430, set = 0, binding = 5) restrict readonly buffer ShadowSSBO
{
	uvec4 uNumShadowMaps;
	mat4 uDirViewProjections[MAX_LIGHTS];
	PointLightShadowInfo uPointShadowInfo[MAX_LIGHTS];
	mat4 uSpotViewProjections[MAX_LIGHTS];
};