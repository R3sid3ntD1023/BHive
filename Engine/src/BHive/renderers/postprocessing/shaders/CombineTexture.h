static const char *combine_texture_comp = R"(
    #version 460 core

    #define LOCAL_SIZE 1
    layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

    layout(binding = 0) uniform sampler2D u_TextureA;
    layout(binding = 1) uniform sampler2D u_TextureB;

    layout(binding = 0, rgba32f) uniform image2D uOutput;

    void main()
    {
        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

        vec2 uv;
        uv.x = (float(texelCoord.x) + .5)/ float(gl_NumWorkGroups.x);
        uv.y = (float(texelCoord.y) + .5)/ float(gl_NumWorkGroups.y);

        vec4 color_a = texture(u_TextureA, uv );
        vec4 color_b = texture(u_TextureB, uv );

        vec4 out_color = color_a + color_b;

        imageStore(uOutput, texelCoord, out_color);
    }
)";