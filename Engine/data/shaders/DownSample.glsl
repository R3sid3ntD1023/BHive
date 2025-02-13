#type compute

#version 460 core
#extension GL_ARB_bindless_texture : require

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;


layout(r11f_g11f_b10f, binding =  0 ) uniform restrict writeonly image2D uImgOutput;
layout(binding = 0) uniform sampler2D u_src_texture;
layout(location = 0) uniform ivec2 u_src_resolution;

void main() {

    vec2 srctexelSize = 1.0 / u_src_resolution;
    float x = srctexelSize.x;
    float y = srctexelSize.y;

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    vec2 uv = (vec2(texelCoord) + .5) / vec2(gl_NumWorkGroups);

    vec3 a = texture(u_src_texture, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
    vec3 b = texture(u_src_texture, vec2(uv.x,       uv.y + 2*y)).rgb;
    vec3 c = texture(u_src_texture, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

    vec3 d = texture(u_src_texture, vec2(uv.x - 2*x, uv.y)).rgb;
    vec3 e = texture(u_src_texture, vec2(uv.x,       uv.y)).rgb;
    vec3 f = texture(u_src_texture, vec2(uv.x + 2*x, uv.y)).rgb;

    vec3 g = texture(u_src_texture, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
    vec3 h = texture(u_src_texture, vec2(uv.x,       uv.y - 2*y)).rgb;
    vec3 i = texture(u_src_texture, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

    vec3 j = texture(u_src_texture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 k = texture(u_src_texture, vec2(uv.x + x, uv.y + y)).rgb;
    vec3 l = texture(u_src_texture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 m = texture(u_src_texture, vec2(uv.x + x, uv.y - y)).rgb;

    vec3 downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;
    downsample = max(downsample, 0.0001f);

    imageStore(uImgOutput, texelCoord, vec4(downsample, 1));
}