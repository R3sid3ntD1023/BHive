#type compute
#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(r11f_g11f_b10f, binding = 0) uniform restrict writeonly image2D imgOutput;
layout(binding = 0) uniform sampler2D srcTexture;

void main() {

    ivec2 srcResolution = textureSize(srcTexture, 0);
    vec2 srctexelSize = 1.0 / srcResolution;
    float x = srctexelSize.x;
    float y = srctexelSize.y;

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    vec2 uv = (vec2(texelCoord) + .5) / vec2(gl_NumWorkGroups);

    vec3 a = texture(srcTexture, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
    vec3 b = texture(srcTexture, vec2(uv.x,       uv.y + 2*y)).rgb;
    vec3 c = texture(srcTexture, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

    vec3 d = texture(srcTexture, vec2(uv.x - 2*x, uv.y)).rgb;
    vec3 e = texture(srcTexture, vec2(uv.x,       uv.y)).rgb;
    vec3 f = texture(srcTexture, vec2(uv.x + 2*x, uv.y)).rgb;

    vec3 g = texture(srcTexture, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
    vec3 h = texture(srcTexture, vec2(uv.x,       uv.y - 2*y)).rgb;
    vec3 i = texture(srcTexture, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

    vec3 j = texture(srcTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(uv.x + x, uv.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(uv.x + x, uv.y - y)).rgb;

    vec3 downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;
    downsample = max(downsample, 0.0001f);

    imageStore(imgOutput, texelCoord, vec4(downsample, 1));
}