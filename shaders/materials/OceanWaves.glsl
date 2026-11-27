
#include <Noise.glsl>

// Returns height in 0..1 (relative to the crest) and the surface slope in `grad`.
float OceanWaves(int count, vec2 p, vec2 mainDir, float time, float steepness, float dist, out vec2 grad) {
    grad = vec2(0.0);
    float h = 0.0, total = 0.0;
    for (int i = 0; i < count; ++i) {
        float fi = float(i);
        float angle = (Hash21(vec2(fi, 3.7)) - 0.5) * 1.5;
        vec2 d = vec2(mainDir.x * cos(angle) - mainDir.y * sin(angle), mainDir.x * sin(angle) + mainDir.y * cos(angle));
        float wavelength = 70.0 / pow(1.62, fi);
        float k = 6.2831853 / wavelength;
        float omega = sqrt(9.81 * k);
        float amp = steepness / k;
        // Short waves fade with distance so the far sea does not shimmer.
        float fade = 1.0 - smoothstep(0.0, 1.0, k * dist * 0.012 - 0.6);
        float phase = k * dot(d, p) - omega * time;
        float e = exp(sin(phase) - 1.0);
        h += amp * e * fade;
        total += amp;
        grad += amp * e * cos(phase) * k * d * fade;
    }
    return h / max(total, 1e-4);
}