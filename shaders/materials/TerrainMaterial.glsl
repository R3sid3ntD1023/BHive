#type vertex
#version 460 core

#include <ForwardMesh.vert>

#type fragment
#version 460 core

#include <Noise.glsl>
#include <Core.glsl>
#include <Lighting.glsl>
#include <ShadowBuffer.glsl>

layout(location = 0) in struct VS_OUT
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Normal;
	vec4 Color;
	mat3 TBN;
	vec3 CameraPosition;
	vec3 DebugColor;
	mat4 View;
} vs_in;

layout(push_constant) uniform MaterialInfo
{
	float uWetness;
} pc;

struct TerrainMaterial
{
    vec3 Diffuse;
	vec3 Normal;
    float Roughness;
    float Metalness;
};

TerrainMaterial GetMaterial(VS_OUT vs)
{
	vec3 wp = vs_in.Position;
    vec3 n = normalize(vs_in.TBN[2]);
    vec2 p = wp.xz;
    float large = Fbm(p * 0.018);
    float mid = Fbm(p * 0.12 + 7.0);
    float fine = Fbm(p * 1.7);
    float grain = ValueNoise(p * 9.0);

    vec3 grassA = vec3(0.035, 0.11, 0.025), grassB = vec3(0.09, 0.15, 0.04);
    vec3 grass = mix(grassA, grassB, smoothstep(0.35, 0.7, mid)) * (0.75 + 0.5 * fine);
    vec3 dry = vec3(0.16, 0.13, 0.05) * (0.7 + 0.6 * fine);
    vec3 mud = vec3(0.07, 0.045, 0.028) * (0.7 + 0.6 * grain);
    vec3 sand = vec3(0.34, 0.27, 0.15) * (0.85 + 0.3 * grain);
    vec3 rock = vec3(0.16, 0.155, 0.16) * (0.6 + 0.8 * fine) * (0.85 + 0.3 * ValueNoise(p * 0.8));

    vec3 c = mix(grass, dry, smoothstep(0.55, 0.75, large));
    c = mix(c, mud, smoothstep(0.62, 0.72, Fbm(p * 0.035 + 31.0)));
    float rough = 0.92;

    float sandMask = 1.0 - smoothstep(0.6, 1.8 + 0.8 * mid, wp.y);
    c = mix(c, sand, sandMask);
    rough = mix(rough, 0.85, sandMask);

    float slope = 1.0 - clamp(n.y, 0.0, 1.0);
    float rockMask = smoothstep(0.22, 0.4, slope + 0.06 * (fine - 0.5));
    c = mix(c, rock, rockMask);
    rough = mix(rough, 0.7, rockMask);

    // Rain collects in puddles on flat ground above the waterline.
    float puddle = smoothstep(0.6, 1.0, pc.uWetness) * smoothstep(0.66, 0.72, Fbm(p * 0.06 + 11.0)) * smoothstep(0.95, 0.995, n.y) * smoothstep(0.4, 0.9, wp.y);
    c *= 1.0 - 0.35 * puddle;
    rough = mix(rough, 0.05, puddle);

	TerrainMaterial mat;
	mat.Diffuse = c;
	mat.Normal = gl_FrontFacing ? n : -n;
    mat.Roughness = 0.5;
	mat.Metalness = 0.0;

	return mat;
}

void Direct_Terrain(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight directLight, const in TerrainMaterial material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.Diffuse.rgb; 
	vec3 irradiance = max(dot(geoNormal, directLight.Direction), 0.0) * diffuse;
	reflectedLight.DirectDiffuse += irradiance * directLight.Color;
}

//#define DIRECTIONAL_SHADOW_MAPPING
//#define POINT_SHADOW_MAPPING
//#define SPOT_SHADOW_MAPPING
#define Direct Direct_Terrain
#define Material TerrainMaterial

#include <ForwardMesh.frag>