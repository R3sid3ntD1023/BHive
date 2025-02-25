static const char *prefiler_comp = R"(
    #version 460 core
    #extension GL_ARB_bindless_texture : require
    #extension GL_NV_uniform_buffer_std430_layout : require

    layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

    layout(std430, binding = 8) uniform PreFilter
    {
        sampler2D uSrcTexture;
        layout(r11f_g11f_b10f) image2D uOutput;
        vec4 uThreshold;
    };

    #define EPSILON 1.0e-4

    vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve);
    float Max3(float a, float b, float c);

    void main()
    {
        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
        float x = float(texelCoord.x)/(gl_NumWorkGroups.x);
        float y = float(texelCoord.y)/(gl_NumWorkGroups.y);

        vec4 value = texture(uSrcTexture, vec2(x, y));
        vec4 color = QuadraticThreshold(value, uThreshold.a, uThreshold.rgb);
	
        imageStore(uOutput, texelCoord, color);
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

    float Max3(float a, float b, float c)
    {
        return max(a, max(b , c));
    }
)";