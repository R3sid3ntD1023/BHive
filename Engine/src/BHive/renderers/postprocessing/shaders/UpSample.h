static const char *upsample_comp = R"(
    #version 460 core
    #extension GL_EXT_scalar_block_layout: require

    #define LOCAL_SIZE 1
    layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

    layout(binding = 0) uniform sampler2D uSrcTexture;
    layout(binding = 0, r11f_g11f_b10f) uniform image2D uOutput;

#ifdef VULKAN
    layout(push_constant) uniform BloomSettings
    {
        float u_FilterRadius;
    } constants;

#else
    layout(location = 0) uniform struct PushConstants
    {
        float u_FilterRadius;
    } constants;

#endif

    void main()
    {
        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

        vec2 uv;
        uv.x = (float(texelCoord.x) + .5)/ float(gl_NumWorkGroups.x);
        uv.y = (float(texelCoord.y) + .5)/ float(gl_NumWorkGroups.y);

        vec3 color = texture(uSrcTexture, uv ).rgb;

        float x = constants.u_FilterRadius;
        float y = constants.u_FilterRadius;

        vec3 upsample = vec3(0);
        upsample += texture(uSrcTexture, vec2(uv.x - x, uv.y + y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x,     uv.y + y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x + x, uv.y + y)).rgb;
  
        upsample += texture(uSrcTexture, vec2(uv.x - x, uv.y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x,     uv.y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x + x, uv.y)).rgb;
     
        upsample += texture(uSrcTexture, vec2(uv.x - x, uv.y - y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x,     uv.y - y)).rgb;
        upsample += texture(uSrcTexture, vec2(uv.x + x, uv.y - y)).rgb;

  
        upsample *= (1.0 / 16.0);

        imageStore(uOutput, texelCoord, vec4(color + upsample, 1));
    }
)";