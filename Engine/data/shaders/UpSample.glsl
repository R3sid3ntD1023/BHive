#type compute
#version 460 core


layout (local_size_x = 1, local_size_y =1, local_size_z = 1) in;


layout(binding = 0) uniform sampler2D srcTexture;
layout(r11f_g11f_b10f, binding = 0) uniform image2D imgOutput;

layout(location = 0) uniform float u_filterRadius;


void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 resolution = textureSize(srcTexture, 0);
    vec2 texelSize = 1.0 / vec2(resolution);

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

    memoryBarrierShared();
    barrier();
   
    imageStore(imgOutput, texelCoord, vec4(color + upsample, 1));
}