#define PI 3.14159265359
#define POS_X 0
#define NEG_X 1
#define POS_Y 2
#define NEG_Y 3
#define POS_Z 4
#define NEG_Z 5

float DisributionGXX(vec3 N, vec3 H, float roughness)
{
	float a2 = roughness * roughness * roughness * roughness;
	float ndoth = max(dot(N, H), 0.0);
	float ndoth2 = ndoth * ndoth;

	float denom = (ndoth2 * (a2 - 1.0) + 1.0);
	return a2 /  (PI * denom * denom) + 0.0001;

}

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0x0F0F0F0Fu) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits)  * 2.3283064365386963e-10;
}

vec2 Hamemersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	vec3 up = abs(N.z) < 0.999 ? vec3(0 ,0 ,1) : vec3(1, 0, 0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

//https://github.com/diharaw/runtime-ibl/blob/master/src/shader/prefilter_cs.glsl

float unlerp(float val, float max_val)
{

	return (val + 0.5) / max_val;
}

vec3 CalculateDirection(uint face, uint face_x, uint face_y, float w, float h)
{
	float s = unlerp(float(face_x), w) * 2.0 - 1.0;
	float t = unlerp(float(face_y), h) * 2.0 - 1.0;
	float x, y ,z;

	switch(face)
	{
		case POS_Z:
			x = s;
			y = -t;
			z = 1;
			break;
		case NEG_Z:
			x = -s;
			y = -t;
			z = -1;
			break;
		case NEG_X:
			x = -1;
			y = -t;
			z = s;
			break;
		case POS_X:
			x = 1;
			y = -t;
			z = -s;
			break;
		case POS_Y:
			x = s;
			y = 1;
			z = t;
			break;
		case NEG_Y:
			x = s;
			y = -1;
			z = -t;
			break;
		default:
		break;
	}

	vec3 d;
	float inv_len = 1.0 /sqrt(x * x + y * y + z * z);
	d.x = x * inv_len;
	d.y = y * inv_len;
	d.z = z * inv_len;

	return d;
}