#type compute
#version 460 core

#define LOCAL_SIZE 1

layout(local_size_x = LOCAL_SIZE,local_size_y = LOCAL_SIZE,local_size_z = LOCAL_SIZE) in;

layout(binding = 0) uniform sampler2D color_texture_sampler;
layout(binding = 0, rgba8) uniform image2D output_texture;

#define OUTLINE_THICKNESS 2
#define EPSILON 0.001

#ifndef __cplusplus
bool is_nearly_zero(float v)
{
    return abs(v) < EPSILON;
}
#endif

void main()
{
	ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);

    vec2 tex_coord;
    tex_coord.x = (float(texel_coord.x) + .5)/ float(gl_NumWorkGroups.x);
    tex_coord.y = (float(texel_coord.y) + .5)/ float(gl_NumWorkGroups.y);

    ivec2 texture_size = textureSize(color_texture_sampler, 0).xy;
    vec2 texel_size = vec2(1.0 / texture_size.x, 1.0 / texture_size.y);

    vec4 color = vec4(0.0);
    int samples = 2 * OUTLINE_THICKNESS + 1;
    for(int x = 0; x < samples; x++)
    {
        for(int y = 0; y < samples; y++)
        {
            vec2 offset = vec2(x - OUTLINE_THICKNESS, y - OUTLINE_THICKNESS);
            color += texture(color_texture_sampler, tex_coord + offset * texel_size);
        }
    }

    color /= samples * samples;
    float alpha =  texture(color_texture_sampler, tex_coord).a;
    color.a = !is_nearly_zero(color.r) && is_nearly_zero(alpha) ? 1.0 : 0.0;

    imageStore(output_texture, texel_coord, color);
}
