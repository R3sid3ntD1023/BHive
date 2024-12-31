#type compute
#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(r11f_g11f_b10f, binding = 0) uniform image2D imgOutput;
layout(binding = 0) uniform sampler2D u_scene_texture;

layout(location = 0) uniform vec4 u_threshold;

#define EPSILON 1.0e-4

vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve);
float Max3(float a, float b, float c);

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    float x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    float y = float(texelCoord.y)/(gl_NumWorkGroups.y);

    vec4 value = texture(u_scene_texture, vec2(x, y));
    vec4 color = QuadraticThreshold(value, u_threshold.a, u_threshold.rgb);
	
    imageStore(imgOutput, texelCoord, color);
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