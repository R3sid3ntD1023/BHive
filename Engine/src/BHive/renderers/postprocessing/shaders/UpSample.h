static const char *upsample_comp = R"(
    #version 460 core
    #extension GL_ARB_bindless_texture : require
    #extension GL_NV_uniform_buffer_std430_layout : require

    #define LOCAL_SIZE 1
    layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

    layout(std430, binding = 10) uniform UpSampler
    {
        sampler2D uSrcTexture;
        layout(r11f_g11f_b10f) image2D uOutput;
        float uFilterRadius;
    };


    void main()
    {
        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

        vec2 uv;
        uv.x = (float(texelCoord.x) + .5)/ float(gl_NumWorkGroups.x);
        uv.y = (float(texelCoord.y) + .5)/ float(gl_NumWorkGroups.y);

        vec3 color = texture(uSrcTexture, uv ).rgb;

        float x = uFilterRadius;
        float y = uFilterRadius;

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