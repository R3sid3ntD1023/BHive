#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0 ) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, r11f_g11f_b10f) uniform image2D uOutput;


void main() {

    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 srcSize = textureSize(uSrcTexture, 0);
  
    vec2 uv = (vec2(dstCoord) + .5) / vec2(gl_NumWorkGroups);
    vec2 texel = 1.0 /  vec2(srcSize);

    vec3 a = texture(uSrcTexture, uv + texel * vec2(-2, 2)).rgb;
    vec3 b = texture(uSrcTexture, uv + texel * vec2(0,  2)).rgb;
    vec3 c = texture(uSrcTexture, uv + texel * vec2(2 , 2)).rgb;
                                 
    vec3 d = texture(uSrcTexture, uv + texel * vec2(-2, 0)).rgb;
    vec3 e = texture(uSrcTexture, uv + texel * vec2(0,  0)).rgb;
    vec3 f = texture(uSrcTexture, uv + texel * vec2(2,  0)).rgb;
                                
    vec3 g = texture(uSrcTexture, uv + texel * vec2(-2, -2)).rgb;
    vec3 h = texture(uSrcTexture, uv + texel * vec2(0,  -2)).rgb;
    vec3 i = texture(uSrcTexture, uv + texel * vec2(2,  -2)).rgb;
                                
    vec3 j = texture(uSrcTexture, uv + texel * vec2(-1, 1)).rgb;
    vec3 k = texture(uSrcTexture, uv + texel * vec2(1, 1)).rgb;
    vec3 l = texture(uSrcTexture, uv + texel * vec2(-1, -1)).rgb;
    vec3 m = texture(uSrcTexture, uv + texel * vec2(1, -1)).rgb;

    vec3 downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;
    downsample = max(downsample, 0.0001f);

    imageStore(uOutput, dstCoord, vec4(downsample, 1));
}