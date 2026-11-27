#type vertex
#version 460 core

#include <OceanWaves.glsl>

#define APPLY_POSITION_OFFSET

layout(push_constant) uniform MaterialInfo
{
	vec2 WindDirection;
    vec2 ShoreCenter;
    float ShoreExtent;
} pc;

vec3 ProcessVertex(vec3 position, float time, vec3 cameraPos)
{
    vec3 wp = position;
    vec2 p = wp.xz;
    float dist = length(cameraPos - wp);

    float windLen = length(pc.WindDirection);
    vec2 mainDir = windLen > 0.01 ? pc.WindDirection / windLen : vec2(1.0, 0.0);
    float windN = clamp((windLen - 0.3) / 1.5, 0.0, 1.0); // 0 calm .. 1 storm
    float steepness = mix(0.035, 0.11, windN);

    vec2 grad;
    float hn = OceanWaves(3, p, mainDir, time, steepness, dist, grad);

    return vec3(0, hn, 0);
}

#include <ForwardMesh.vert>

#type fragment
#version 460 core

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
    float Time;
} vs_in;

layout(push_constant) uniform MaterialInfo
{
	vec2 WindDirection;
    vec2 ShoreCenter;
    float ShoreExtent;
} pc;

layout(set = SET_MATERIAL, binding = 0) uniform sampler2D ShoreMap;

#include <OceanWaves.glsl>

float SampleShoreHeight(vec2 uv)
{
    ivec2 size = textureSize(ShoreMap, 0);
    vec2 texelPosition = uv * vec2(size) - 0.5;
    ivec2 texel = ivec2(floor(texelPosition));
    ivec2 lastTexel = size - 1;
    vec2 blend = fract(texelPosition);

    float h00 = texelFetch(ShoreMap, clamp(texel, ivec2(0), lastTexel), 0).r;
    float h10 = texelFetch(ShoreMap, clamp(texel + ivec2(1, 0), ivec2(0), lastTexel), 0).r;
    float h01 = texelFetch(ShoreMap, clamp(texel + ivec2(0, 1), ivec2(0), lastTexel), 0).r;
    float h11 = texelFetch(ShoreMap, clamp(texel + ivec2(1, 1), ivec2(0), lastTexel), 0).r;
    return mix(mix(h00, h10, blend.x), mix(h01, h11, blend.x), blend.y);
}

struct Surface
{
    vec3 Albedo;
    float Opacity;
    float Metalness;
    float Roughness;
    vec3 Normal;
    vec3 IrradianceF0;
};

#define USE_ENVIRONMENT_MAPS

layout(set = SET_IMAGE_BASED_LIGHTING, binding = 0) uniform sampler2D BRDFLutMap;
layout(set = SET_IMAGE_BASED_LIGHTING, binding = 1) uniform samplerCube PreFilterMap;
layout(set = SET_IMAGE_BASED_LIGHTING, binding = 2) uniform samplerCube IrradianceMap;

Surface GetMaterial(VS_OUT vs) 
{
    float time = vs.Time;
    vec3 wp = vs.Position;
    vec2 p = wp.xz;
    float dist = length(vs.CameraPosition - wp);

    vec2 uv = (p - pc.ShoreCenter) / pc.ShoreExtent + 0.5;
    float terrainHeight = -10.0;
    if (all(greaterThanEqual(uv, vec2(0.0))) && all(lessThanEqual(uv, vec2(1.0))))
        terrainHeight = SampleShoreHeight(uv);

    float depth = vs.Position.y - terrainHeight;
    float shoreOpacity = smoothstep(0.0, 1.5, depth);

    float windLen = length(pc.WindDirection);
    vec2 mainDir = windLen > 0.01 ? pc.WindDirection / windLen : vec2(1.0, 0.0);
    float windN = clamp((windLen - 0.3) / 1.5, 0.0, 1.0); // 0 calm .. 1 storm
    float steepness = mix(0.035, 0.11, windN);

    vec2 grad;
    float hn = OceanWaves(8, p, mainDir, time, steepness, dist, grad);

    // Fine ripples drifting with the wind.
    vec2 q = p * 0.9 + mainDir * time * 0.35;
    float r0 = Fbm(q), rx = Fbm(q + vec2(0.07, 0.0)), rz = Fbm(q + vec2(0.0, 0.07));
    grad += vec2(rx - r0, rz - r0) * mix(1.6, 3.0, windN) * (1.0 - smoothstep(40.0, 260.0, dist));

    grad *= mix(1.0, 0.4, smoothstep(150.0, 600.0, dist));
    vec3 N = normalize(vec3(-grad.x, 1.0, -grad.y));

    if (!gl_FrontFacing) {
        // Seen from below: inside Snell's window (about 49 degrees around straight up) the sky shows through,
        // refracted and tinted; beyond it the surface is a mirror of the dark water.
        vec3 rayUp = -normalize(vs.CameraPosition - wp);
        vec3 refr = refract(rayUp, -N, 1.33);
        vec3 tir = vec3(0.006, 0.04, 0.05);
        vec3 sky = vec3(0.0);
        float window = 0.0;
        if (dot(refr, refr) > 0.0) {
            sky = textureLod(PreFilterMap, normalize(refr), 1.0).rgb * vec3(0.35, 0.8, 0.9);
            window = smoothstep(0.0, 0.08, length(refr));
            // The window edge brightens where light grazes the surface (Fresnel), then fades into the mirror.
            window *= 1.0 - smoothstep(0.55, 0.97, 1.0 - dot(rayUp, N));
        }
      //  float shimmer = 0.85 + 0.3 * Fbm(p * 1.3 + vec2(time * 0.4, -time * 0.3));
      //  gSurfaceEmission = mix(tir, sky * shimmer, clamp(window, 0.0, 1.0));

        Surface under;
        under.Albedo = mix(tir, sky, clamp(window, 0.0, 1.0));
        under.Opacity = 0.9 * shoreOpacity;
        under.Metalness = 0.0;
        under.Roughness = 0.35;
        under.Normal = -N;
        under.IrradianceF0 = vec3(0.04);
        return under;
    }

   

    // Terrain below the surface: shallow water is lighter and turquoise, and breaks into foam near the shore.
    float shallow = 1.0 - smoothstep(0.0, 7.0, depth);

    float depthAA = max(fwidth(depth), 0.1);
    float shallowEdge = 1.0 - smoothstep(-depthAA, depthAA, depth);
    float deepEdge = smoothstep(1.6 - depthAA, 1.6 + depthAA, depth);
    float band = shallowEdge * (1.0 - deepEdge);

    float linePhase = depth * 2.5 - time * 1.6 + Fbm(p * 0.2) * 2.0;
    float lineVisibility = 1.0 - smoothstep(0.5, 1.5, fwidth(linePhase));
    float lines = 0.5 + 0.5 * sin(linePhase) * lineVisibility;
    float surfDistance = abs(depth - 2.4 - sin(time * 0.7) * 0.8);
    float surfWidth = max(0.9, depthAA * 2.0);
    float surf = 1.0 - smoothstep(0.0, surfWidth, surfDistance);
    float shoreDetail = Fbm(p * 0.18 + time * 0.05);
    float shoreFoam = clamp(band * (0.45 + 0.55 * lines) + surf * 0.35 * smoothstep(0.35, 0.7, shoreDetail), 0.0, 1.0);

    // Whitecaps on the highest crests, more of them as the wind rises.
    float thresh = mix(0.97, 0.66, windN);
    float capSignal = hn + (Fbm(p * 0.12 + time * 0.1) - 0.5) * 0.3;
    float capAA = max(fwidth(capSignal), 0.02);
    float caps = smoothstep(thresh - capAA, thresh + 0.1 + capAA, capSignal);
    float capWeight = smoothstep(0.45, 0.85, windN);
    float foam = clamp(max(shoreFoam, caps * capWeight), 0.0, 1.0);

    vec3 deep = vec3(0.006, 0.055, 0.14);
    vec3 shallowColor = vec3(0.015, 0.32, 0.42);
    vec3 water = mix(deep, shallowColor, shallow * 0.9);
    water += vec3(0.0, 0.04, 0.035) * hn;                 // light scattering through thin crests
    water = mix(water, vec3(0.035, 0.09, 0.13), windN * 0.5); // storm sea turns grey-blue

    Surface s;
    s.Albedo = mix(water, vec3(0.85, 0.88, 0.9), foam);
    s.Opacity = 0.78 * shoreOpacity;
    s.Metalness = 0.0;
    s.Roughness = mix(mix(0.035, 0.12, windN), 0.75, foam);
    s.Normal = N;
    s.IrradianceF0 = mix(vec3(0.04), s.Albedo, s.Metalness);
    return s;
}

#include <BDRFFunctions.glsl>

void Direct_Ocean(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight light, const in Surface material, inout ReflectedLight reflectedLight)
{
	vec3 bdrf = CalculateBDRF(material.IrradianceF0, geoNormal, geoViewDir, light.Direction, material.Albedo, material.Metalness, material.Roughness);

	float NdotL = max(dot(geoNormal, normalize(light.Direction)), 0.0);
	reflectedLight.DirectDiffuse += (bdrf * light.Color * NdotL);
}

void InDirect_Ocean(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir,const in Surface material, inout ReflectedLight reflectedLight)
{
	vec3 F = fresnelSchlickRoughness(max(dot(geoNormal,geoViewDir), 0.0), material.IrradianceF0, material.Roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.Metalness;
	reflectedLight.IndirectDiffuse = kD *  irradiance * material.Albedo;
}

void InDirectSpecular_Ocean(const in vec3 irradiance, const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in Surface material, inout ReflectedLight reflectedLight)
{
	reflectedLight.IndirectSpecular =  irradiance;
}


//#define DIRECTIONAL_SHADOW_MAPPING
//#define POINT_SHADOW_MAPPING
//#define SPOT_SHADOW_MAPPING
#define Direct Direct_Ocean
#define InDirect InDirect_Ocean
#define InDirectSpecular InDirectSpecular_Ocean
#define Material Surface
#define HAS_OPACITY

#include <ForwardMesh.frag>