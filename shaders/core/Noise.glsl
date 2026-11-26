float Hash21(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float ValueNoise(vec2 p) {
    vec2 i = floor(p), f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(Hash21(i), Hash21(i + vec2(1, 0)), f.x),
               mix(Hash21(i + vec2(0, 1)), Hash21(i + vec2(1, 1)), f.x), f.y);
}

float Fbm(vec2 p) {
    float v = 0.0, a = 0.5;
    for (int i = 0; i < 4; ++i) {
        v += a * ValueNoise(p);
        p = p * 2.03 + 17.0;
        a *= 0.5;
    }
    return v;
}
