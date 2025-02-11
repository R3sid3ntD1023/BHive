#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;


layout(binding = 0) uniform sampler2D srcTexture;
layout(r11f_g11f_b10f, binding = 0) uniform image2D imgOutput;

layout(location = 0) uniform float u_filterRadius;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv;
    uv.x = (float(texelCoord.x) + .5)/ float(gl_NumWorkGroups.x);
    uv.y = (float(texelCoord.y) + .5)/ float(gl_NumWorkGroups.y);

    vec3 color = texture(srcTexture, uv ).rgb;

    float x = u_filterRadius;
    float y = u_filterRadius;

    vec3 upsample = vec3(0);
    upsample += texture(srcTexture, vec2(uv.x - x, uv.y + y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x,     uv.y + y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x + x, uv.y + y)).rgb;
  
    upsample += texture(srcTexture, vec2(uv.x - x, uv.y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x,     uv.y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x + x, uv.y)).rgb;
     
    upsample += texture(srcTexture, vec2(uv.x - x, uv.y - y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x,     uv.y - y)).rgb;
    upsample += texture(srcTexture, vec2(uv.x + x, uv.y - y)).rgb;

  
    upsample *= (1.0 / 16.0);

    imageStore(imgOutput, texelCoord, vec4(color + upsample, 1));
}