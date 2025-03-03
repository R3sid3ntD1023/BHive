#type vertex
#version 460 core

#include <Skinning.glsl>
#include <Core.glsl>

#ifdef USE_VERTEX_PULLING

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	float Position[3];
	float TexCoord[2];
	float Normal[3];
	float Tangent[3];
	float BiNormal[3];
	float Color[4];
	int BoneIds[MAX_BONE_INFLUENCE];
	float Weights[MAX_BONE_INFLUENCE];
};

layout(std430, binding = 0) restrict readonly buffer VertexSSBO
{
	Vertex in_vertices[];
};

#else

layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

#endif

layout(std430, binding = 0) uniform Camera
{
	mat4 uProjection;
	mat4 uView;
	vec2 uNearFar;
};

struct PerObjectData
{ 
	mat4 WorldMatrix;
};


layout(std430, binding = 1) restrict readonly buffer ObjectSSBO
{
	PerObjectData o[];
};

layout(std430, binding = 2) restrict readonly buffer InstanceSSBO
{
	mat4 instances[];
};

layout(location  = 0) out struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
} vs_out;

void main()
{

#ifdef USE_VERTEX_PULLING
	Vertex v = in_vertices[gl_VertexID];
	vec3 vPosition = vec3(v.Position[0],v.Position[1],v.Position[2]);
	vec3 vNormal = vec3(v.Normal[0],v.Normal[1],v.Normal[2]);
	ivec4 vBoneIds = ivec4(v.BoneIds[0], v.BoneIds[1], v.BoneIds[2],v.BoneIds[3]);
	vec4 vWeights = vec4(v.Weights[0], v.Weights[1], v.Weights[2],v.Weights[3]);

#endif

	vec4 pos = vec4(vPosition , 1);
	mat4 boneTransform = Skinning(vWeights, vBoneIds);
	vec4 posL = boneTransform * pos;

	bool instanced = gl_InstanceID != -1; 

	mat4 instance = mix(mat4(1), instances[gl_InstanceID], float(instanced));

	mat4 model =  o[gl_DrawID].WorldMatrix * instance;
	gl_Position = uProjection * uView *  model *  posL;

	vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
	vs_out.Position = vec3(model * posL);
}

#type fragment
#version 460 core

#include <Core.glsl>
#include <Lighting.glsl>

vec3 lightPos = vec3(-5, 10, 0);


layout(location  = 0) in struct VS_OUT
{
	vec3 Position;
	vec3 Normal;
} vs_in;

layout(bindless_sampler) uniform sampler2DArrayShadow uDirectionalShadowMaps;
layout(bindless_sampler) uniform samplerCubeArray uPointShadowMaps;
layout(bindless_sampler) uniform sampler2DArray uSpotShadowMaps;

layout(location = 0) out vec4 fColor;


void main()
{
	vec3 P = vs_in.Position;
	vec3 N = normalize(vs_in.Normal);
	vec3 Lo = vec3(0);
	
	int j =0, k = 0, s= 0;
	for(int i = 0; i < uNumLights; i++)
	{
		Light light = uLights[i];
		uint type = light.type;

		switch(type)
		{
			case 0:
			{
				vec3 l = DirectionalLight(N, light.direction) * light.color  * light.brightness;
				float shadow = DirLightShadow(j++, P, uDirectionalShadowMaps);
				l *= shadow;
				Lo += l;
				break;
			}
			case 1:
			{
				vec3 L = light.position - P;
				vec3 l = PointLight( P, N , light.position, light.radius) * light.color * light.brightness;
				float shadow = SampleVariancePointLightShadow(k++, P, normalize(L), L, vec2(1.f, light.radius), uPointShadowMaps);
				l *= shadow;
				Lo += l;
				break;
			}
			case 2:
			{
				vec3 l = SpotLight(P, N, light.position, light.direction, light.radius, light.outerCutoff, light.innerCutoff) *  light.color  * light.brightness;
				float shadow = SampleVarianceSpotLightShadow(s++, P, uSpotShadowMaps);
				l *= shadow;
				Lo += l;
				break;
			}
			default:
			break;
		}
		
	}

	vec3 color = vec3(1) * Lo;
	fColor = vec4(color, 1);
}