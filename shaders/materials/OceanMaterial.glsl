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
    float Time;
	vec2 WindDirection;
    vec2 ShoreCenter;
    float ShoreExtent;
} pc;

layout(set = SET_MATERIAL, binding = 0) uniform sampler2D ShoreMap;

const int kWaves = 8;

// Returns height in 0..1 (relative to the crest) and the surface slope in `grad`.
float OceanWaves(vec2 p, vec2 mainDir, float steepness, float dist, out vec2 grad) {
    grad = vec2(0.0);
    float h = 0.0, total = 0.0;
    for (int i = 0; i < kWaves; ++i) {
        float fi = float(i);
        float angle = (Hash21(vec2(fi, 3.7)) - 0.5) * 1.5;
        vec2 d = vec2(mainDir.x * cos(angle) - mainDir.y * sin(angle), mainDir.x * sin(angle) + mainDir.y * cos(angle));
        float wavelength = 70.0 / pow(1.62, fi);
        float k = 6.2831853 / wavelength;
        float omega = sqrt(9.81 * k);
        float amp = steepness / k;
        // Short waves fade with distance so the far sea does not shimmer.
        float fade = 1.0 - smoothstep(0.0, 1.0, k * dist * 0.012 - 0.6);
        float phase = k * dot(d, p) - omega * pc.Time;
        float e = exp(sin(phase) - 1.0);
        h += amp * e * fade;
        total += amp;
        grad += amp * e * cos(phase) * k * d * fade;
    }
    return h / max(total, 1e-4);
}

struct Surface
{
    vec3 Diffuse;
    float Alpha;
    float Metalness;
    float Roughness;
    vec3 Normal;
};

#define USE_ENVIRONMENT_MAPS

layout(set = SET_IMAGE_BASED_LIGHTING, binding = 1) uniform samplerCube PrefilterMap;

Surface GetMaterial(VS_OUT vs) 
{
    float time = pc.Time;
    vec3 wp = vs.Position;
    vec2 p = wp.xz;
    float dist = length(vs.CameraPosition - wp);

    float windLen = length(pc.WindDirection);
    vec2 mainDir = windLen > 0.01 ? pc.WindDirection / windLen : vec2(1.0, 0.0);
    float windN = clamp((windLen - 0.3) / 1.5, 0.0, 1.0); // 0 calm .. 1 storm
    float steepness = mix(0.035, 0.11, windN);

    vec2 grad;
    float hn = OceanWaves(p, mainDir, steepness, dist, grad);

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
            sky = textureLod(PrefilterMap, normalize(refr), 1.0).rgb * vec3(0.35, 0.8, 0.9);
            window = smoothstep(0.0, 0.08, length(refr));
            // The window edge brightens where light grazes the surface (Fresnel), then fades into the mirror.
            window *= 1.0 - smoothstep(0.55, 0.97, 1.0 - dot(rayUp, N));
        }
      //  float shimmer = 0.85 + 0.3 * Fbm(p * 1.3 + vec2(time * 0.4, -time * 0.3));
      //  gSurfaceEmission = mix(tir, sky * shimmer, clamp(window, 0.0, 1.0));

        Surface under;
        under.Diffuse = vec3(0.0);
        under.Alpha = 1.0;
        under.Metalness = 0.0;
        under.Roughness = 1.0;
        under.Normal = -N;
        return under;
    }

   

    // Terrain below the surface: shallow water is lighter and turquoise, and breaks into foam near the shore.
    vec2 uv = (p - pc.ShoreCenter) / pc.ShoreExtent + 0.5;
    float depth = max(-texture(ShoreMap, uv).r, 0.0);
    float shallow = 1.0 - smoothstep(0.0, 7.0, depth);

    float lines = 0.5 + 0.5 * sin(depth * 5.0 - time * 1.6 + Fbm(p * 0.5) * 4.0);
    float band = 1.0 - smoothstep(0.0, 1.6, depth);
    float surf = 1.0 - smoothstep(0.0, 0.9, abs(depth - 2.4 - sin(time * 0.7) * 0.8));
    float shoreFoam = clamp(band * (0.45 + 0.55 * lines) + surf * 0.35 * smoothstep(0.35, 0.7, Fbm(p * 0.7 + time * 0.05)), 0.0, 1.0);

    // Whitecaps on the highest crests, more of them as the wind rises.
    float thresh = mix(0.97, 0.66, windN);
    float caps = smoothstep(thresh, thresh + 0.1, hn + (Fbm(p * 0.8 + time * 0.1) - 0.5) * 0.3);
    float foam = clamp(max(shoreFoam, caps * (0.3 + 0.7 * windN)), 0.0, 1.0);

    vec3 deep = vec3(0.004, 0.028, 0.045);
    vec3 shallowColor = vec3(0.02, 0.15, 0.14);
    vec3 water = mix(deep, shallowColor, shallow * 0.9);
    water += vec3(0.0, 0.03, 0.025) * hn;                 // light scattering through thin crests
    water = mix(water, vec3(0.03, 0.05, 0.055), windN * 0.5); // storm sea turns grey-green

    Surface s;
    s.Diffuse = mix(water, vec3(0.85, 0.88, 0.9), foam);
    s.Alpha = 1.0;
    s.Metalness = 0.0;
    s.Roughness = mix(mix(0.035, 0.12, windN), 0.75, foam);
    s.Normal = N;
    return s;
}

void Direct_Surface(const in vec3 geoPosition, const in vec3 geoNormal, const in vec3 geoViewDir, const in IncidentLight directLight, const in Surface material, inout ReflectedLight reflectedLight)
{
	vec3 diffuse = material.Diffuse.rgb; 
	vec3 irradiance = max(dot(geoNormal, directLight.Direction), 0.0) * diffuse;
	reflectedLight.DirectDiffuse += irradiance * directLight.Color;
}

//#define DIRECTIONAL_SHADOW_MAPPING
//#define POINT_SHADOW_MAPPING
//#define SPOT_SHADOW_MAPPING
#define Direct Direct_Surface
#define Material Surface

#include <ForwardMesh.frag>