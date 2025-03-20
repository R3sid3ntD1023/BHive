static const char *aces_comp = R"(
	#version 460 core
    #extension GL_ARB_bindless_texture : require
    #extension GL_NV_uniform_buffer_std430_layout : require

    #define LOCAL_SIZE 1
    layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;


    //https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
    /*
    =================================================================================================

      Baking Lab
      by MJP and David Neubelt
      http://mynameismjp.wordpress.com/

      All code licensed under the MIT license

    =================================================================================================
     The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
     credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)
    */

    // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
    mat3 ACESInputMat = mat3(
      0.59719, 0.35358, 0.04823,
      0.07600, 0.90834, 0.01566,
      0.02840, 0.13383, 0.83777
    );

    // ODT_SAT => XYZ => D60_2_D65 => sRGB
    mat3 ACESOutputMat = mat3
    (
      1.60475, -0.53108, -0.07367,
      -0.10208, 1.10813, -0.00605,
      -0.00327, -0.07276, 1.07602
    );

    vec3 RRTAndODTFit(vec3 v)
    {
        vec3 a = v * (v + 0.0245786f) - 0.000090537f;
        vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
        return a / b;
    }

    vec3 ACESFitted(vec3 color)
    {
        color = transpose(ACESInputMat) * color;

        //Apply RTT and ODT
        color = RRTAndODTFit(color);
        color = transpose(ACESOutputMat) * color;
        color = clamp(color, 0, 1);
        return color;
    }

    vec3 ACES(vec3 color) {
      const float a = 2.51f;
      const float b = 0.03f;
      const float c = 2.43f;
      const float d = 0.59f;
      const float e = 0.14f;
      return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
    }


    vec3 filmic(vec3 x) {
      vec3 X = max(vec3(0.0), x - 0.004);
      vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
      return pow(result, vec3(2.2));
    }

    layout(binding = 0) uniform sampler2D uSrcTexture;
    layout(binding = 0, r11f_g11f_b10f) uniform image2D uOutput;

    void main()
    {
        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
        float x = float(texelCoord.x)/(gl_NumWorkGroups.x);
        float y = float(texelCoord.y)/(gl_NumWorkGroups.y);

        vec4 color = texture(uSrcTexture, vec2(x,y));
        color.rgb = ACES(color.rgb);

        imageStore(uOutput, texelCoord, color);
    }
)";