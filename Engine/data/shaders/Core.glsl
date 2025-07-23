#ifdef VULKAN
#define INSTANCE_ID gl_InstanceIndex
#define IS_INSTANCED (INSTANCE_ID != -1)
#else
#define INSTANCE_ID gl_InstanceID
#define IS_INSTANCED (INSTANCE_ID != 0)
#endif


#define EPSILON 1.0e-4
#define BIT(x) 1 << x
#define UNLIT BIT(0)
#define PI 3.14159265359

bool IsApproximatelyEqual(float a, float b)
{
	return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float Max3(float a, float b, float c)
{
	return max(max(a, b), c);
}

float Max3(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

mat4 mix(mat4 a, mat4 b, float v)
{
	return (a * (1.f - v)) +  (b *  v);
}

float linStep(float low, float high, float v )
{
	return clamp((v-low)/(high - low), 0, 1);
}

float SampleVariance(vec2 moments, float compare)
{
	float p = step(compare, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);
	float d = compare - moments.x;
	float pMax = linStep(0.2, 1, variance / (variance + d*d));
	return min(max(p, pMax), 1.0);
}

float random(in vec3 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
	return fract(sin(dt) * 2105.2354);
}


vec3 CalculateTangentSpaceNormal(vec3 N, mat3 TBN, vec4 normal_map)
{
	//calaculate tangent space normal
	vec3 normal = normal_map.rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);
	return normal;
}

vec4 CalculateTransparencyWeight(vec4 color, out float reveal, float depth)
{
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 *
		pow(1.0 - depth * 0.9, 3.0), 1e-2, 3e3);
		
	vec4 accum = vec4(color.rgb * color.a, color.a) * weight;
	
	reveal = color.a;

	return accum; 
}

vec4 CompositeWeightedTransparency(in sampler2D reveal, in sampler2D accum, ivec2 coords, out bool is_equal)
{
	float revealage = texelFetch(reveal, coords, 0).r;

	if(IsApproximatelyEqual(revealage, 1.0))
	{
		is_equal = false;
	}

	vec4 accumulaton = texelFetch(accum, coords, 0);

	if(isinf(Max3(accumulaton.rgb)))
	{
		accumulaton.rgb = vec3(accumulaton.a);
	}

	vec3 average_color = accumulaton.rgb / max(accumulaton.a, EPSILON);

	return vec4(average_color, 1.0 - revealage);
}

