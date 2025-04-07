static const char *downsample_comp = R"(

    #version 460 core
   #extension GL_EXT_scalar_block_layout: require

    #define LOCAL_SIZE 1
    layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

    layout(binding = 0 ) uniform sampler2D uSrcTexture;
    layout(binding = 0, r11f_g11f_b10f) uniform image2D uOutput;


    void main() {

        vec2 srctexelSize = 1.0 / textureSize(uSrcTexture, 0);
        float x = srctexelSize.x;
        float y = srctexelSize.y;

        ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
        vec2 uv = (vec2(texelCoord) + .5) / vec2(gl_NumWorkGroups);

        vec3 a = texture(uSrcTexture, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
        vec3 b = texture(uSrcTexture, vec2(uv.x,       uv.y + 2*y)).rgb;
        vec3 c = texture(uSrcTexture, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

        vec3 d = texture(uSrcTexture, vec2(uv.x - 2*x, uv.y)).rgb;
        vec3 e = texture(uSrcTexture, vec2(uv.x,       uv.y)).rgb;
        vec3 f = texture(uSrcTexture, vec2(uv.x + 2*x, uv.y)).rgb;

        vec3 g = texture(uSrcTexture, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
        vec3 h = texture(uSrcTexture, vec2(uv.x,       uv.y - 2*y)).rgb;
        vec3 i = texture(uSrcTexture, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

        vec3 j = texture(uSrcTexture, vec2(uv.x - x, uv.y + y)).rgb;
        vec3 k = texture(uSrcTexture, vec2(uv.x + x, uv.y + y)).rgb;
        vec3 l = texture(uSrcTexture, vec2(uv.x - x, uv.y - y)).rgb;
        vec3 m = texture(uSrcTexture, vec2(uv.x + x, uv.y - y)).rgb;

        vec3 downsample = e*0.125;
        downsample += (a+c+g+i)*0.03125;
        downsample += (b+d+f+h)*0.0625;
        downsample += (j+k+l+m)*0.125;
        downsample = max(downsample, 0.0001f);

        imageStore(uOutput, texelCoord, vec4(downsample, 1));
    }
)";