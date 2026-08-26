#define EPSILON 1.0e-4
#define BIT(x) 1 << x
#define UNLIT BIT(0)
#define PI 3.14159265359

struct ObjectData
{	
	mat4 model; //model matrix
	vec4 center_radius;//bounding sphere center
    uint meshIndex; // which mesh this instance belongs to	
};

struct IndirectDrawCommand
{
    uint indexCount;
    uint instanceCount;
    uint indexOffset;
    int vertexOffset;
    uint firstInstance;
};

struct Frustum
{
    vec4 planes[6];
	vec4 points[8];
};

vec4 EncodeEntityID(in int id)
{
	vec4 color = vec4(-1, -1, -1, 1);
	id += 1;
	color.r = (id & 0xFF) / 255.0;
	color.g = ((id << 8) & 0xFF) / 255.0;
	color.b = ((id << 16) & 0xFF) / 255.0;
	return color;
}


float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

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

vec3 SoftKeeThreshold(vec3 color, float threshold)
{
	threshold = max(threshold, 0.0);
    float knee = threshold * 0.5;

    // Pixel brightness
    float br = Max3(color.r, color.g, color.b);

    //soft knee start
    float kneeStart = threshold - knee;

    //amount abouve knee start
    float soft = clamp(br -kneeStart, 0.0 , 2.0 * knee);

    //apply quadratic soft knee
    soft = soft * soft / (4.0 * knee + 1e-4);

    float bloomFactor = max(br - threshold, soft);

    return color * (bloomFactor / max(br, 1e-4));
}

vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve)
{
    // Pixel brightness
    float br = Max3(color.r, color.g, color.b);

    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

    return color;
}

float LinerizeDepth(float depth, float near, float far)
{
    float ndc = 2 * depth - 1;
    float eye = (2 * near * far) / ((far + near) - ndc * (far -near));
    float linearDepth = (eye - near) / (far - near);
    return linearDepth;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float scale, in sampler2D depth)
{
	const float min_layers = 8.0;
	const float max_layers = 32.0;
	float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0, 0, 1), viewDir)));

	float layerDepth = 1.0 / num_layers;

	float current_layer_depth = 0.0;

	vec2 p = viewDir.xy / viewDir.z * scale;
	vec2 dcoords = p / num_layers;

	vec2 current_coords = texCoords;
	float current_depth_map_value = texture(depth, current_coords).r;

	while(current_layer_depth < current_depth_map_value)
	{
		current_coords -= dcoords;
		current_depth_map_value = texture(depth, current_coords).r;

		current_layer_depth += layerDepth;
	}

	vec2 prevTexCoords = current_coords + dcoords;

	float after_depth = current_depth_map_value - current_layer_depth;
	float before_depth = texture(depth, prevTexCoords).r - current_layer_depth + layerDepth;

	float weight = after_depth / (after_depth - before_depth);


	return prevTexCoords * weight + current_coords * (1.0 - weight);
}

//bones
#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

bool HasBones(ivec4 indices)
{
	return indices.x != -1 || indices.y != -1 || indices.z != -1 || indices.w != -1;
}

mat4 GetBoneMatrix(const in vec4 weights, const in ivec4 indices, const in mat4[MAX_BONES] _bones)
{
	if(!HasBones(indices)) return mat4(1.0f);

	mat4 bone_transform = mat4(0.0f);

	for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if(indices[i] == -1)
		{
			continue;
		}

		if(indices[i] >= MAX_BONES)
		{
			break;
		}

		
		bone_transform += _bones[indices[i]] * weights[i];

	}

	return bone_transform;
}
