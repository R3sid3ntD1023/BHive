#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(binding = 0) uniform sampler2D color_texture_sampler;
layout(binding = 1) uniform sampler2D outline_texture_sampler;
layout(binding = 2) uniform sampler3D color_grading_lut_texture_sampler;

layout(binding = 0, rgba32f) uniform image2D output_texture;

#define OUTLINE_COLOR vec4(0.89, 0.61, 0.0003, 1.0)

void main()
{
	ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);

    vec2 tex_coord;
    tex_coord.x = (float(texel_coord.x) + .5)/ float(gl_NumWorkGroups.x);
    tex_coord.y = (float(texel_coord.y) + .5)/ float(gl_NumWorkGroups.y);

	vec4 color = texture(color_texture_sampler, tex_coord);
	ivec3 dim = textureSize(color_grading_lut_texture_sampler, 0);
	if(dim.x > 1)
	{
		vec3 uvw = vec3(1.0 - color.g, color.r, color.b) + vec3(-.5, 0.f, 0.5) / vec3(dim.y, dim.x, dim.z);
		color = texture(color_grading_lut_texture_sampler, uvw);
	}

	if(texture(outline_texture_sampler, tex_coord).a > 0.5)
	{
		color = OUTLINE_COLOR;
	}

	 imageStore(output_texture, texel_coord, color);
}