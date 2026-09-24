#define USE_LIGHTING
#define BLINN_PHONG 1
#define LIGHTING_TYPE BLINN_PHONG
#define MAX_LIGHTS 32
#define USE_POISSONDISK 1

struct DirectionalLight
{
	vec4 Color;			// rgb + intensity
	vec4 Direction;		// xyz + unused
};

struct PointLight
{
	vec4 Color;			// rgb + intensity
	vec4 Position;		// xyz + radius
};

struct SpotLight
{
	vec4 Color;			// rgb + intensity
	vec4 Position;		// xyz + radius
	vec4 Direction;		// xyz + innerCutoff
	vec4 Params;		// outerCutoff + padding
};

struct IncidentLight
{
	vec3 Color;
	vec3 Direction;
};

struct ReflectedLight
{
	vec3 DirectDiffuse;
	vec3 DirectSpecular;
	vec3 IndirectDiffuse;
	vec3 IndirectSpecular;
};


// @semantic Lights
layout(std430, set = 0, binding = 1) restrict readonly buffer LightSSBO
{
	uvec4 NumLights; //dir, point, spot
	DirectionalLight uDirectionalLights[MAX_LIGHTS];
	PointLight uPointLights[MAX_LIGHTS];
	SpotLight uSpotLights[MAX_LIGHTS];
};


void GetDirectionalLightInfo(const in DirectionalLight light, inout IncidentLight directLight)
{
	directLight.Direction = normalize(-light.Direction.xyz);
	directLight.Color = max(vec3(0), light.Color.rgb * light.Color.a);
}

void GetPointLightInfo(const in PointLight light, const in vec3 geoPosition, inout IncidentLight directLight)
{
	float radius = max(light.Position.w, 0.001);

	vec3 L = light.Position.xyz - geoPosition;
	float dist = length(L);
	directLight.Direction = L;

	//https://lisyarus.github.io/blog/posts/point-light-attenuation.html
	float s = dist / radius;

	float fade = 1.0 - smoothstep(0.5, 1.0, s);

	float attenuation = sqrt(max(0.0, 1.0 - sqrt(min(s, 1.0)))) / (1.0 + radius * s);
	directLight.Color = light.Color.rgb * light.Color.a * attenuation * fade;
}

void GetSpotLightInfo(const in SpotLight light, const in vec3 geoPosition, inout IncidentLight directLight)
{
	PointLight point_light = PointLight(light.Color, light.Position);
	GetPointLightInfo(point_light, geoPosition, directLight);
	
	float theta = dot(normalize(light.Position.xyz - geoPosition), normalize(-light.Direction.xyz ));
	float epsilon = light.Direction.w - light.Params.x;
	float intensity = smoothstep(0, 1 , (theta - light.Params.x) / epsilon);
	
	directLight.Color *= intensity;
}

