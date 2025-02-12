#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 8) in mat4 vInstanceMatrix;

layout(std430 , binding = 0) uniform ObjectBuffer{ mat4 u_projection; mat4 u_view; mat4 u_model;} ;

layout(location = 0) out struct VS_OUT
{
    vec3 position;
    vec2 texcoord;
    vec3 normal;
} vs_out;

void main()
{
    vec4 worldPos = u_model * vInstanceMatrix * vec4(vPos, 1);
    vs_out.position = worldPos.xyz;
    vs_out.texcoord = vTexCoord;

    mat3 normal_matrix = transpose(inverse(mat3(u_model * vInstanceMatrix)));
    vs_out.normal = normal_matrix * vNormal;

    gl_Position = u_projection * u_view * worldPos;
}

#type fragment

#version 460 core
#extension GL_ARB_bindless_texture : require

#define SINGLE_CHANNEL 1 << 0

layout(location = 0) in struct VS_OUT
{
    vec3 position;
    vec2 texcoord;
    vec3 normal;
} vs_in;


layout(bindless_sampler) uniform sampler2D u_Texture;

layout(location = 0) uniform uint uFlags = 0;
layout(location = 1) uniform vec3 uColor;
layout(location = 2) uniform vec3 uEmission;


layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPosition;
layout(location = 2) out vec4 fNormal;
layout(location = 3) out vec4 fEmission;

void main()
{
    uint is_single_channel = (uFlags & SINGLE_CHANNEL);

    vec3 color = pow(texture(u_Texture, vs_in.texcoord).rgb, vec3(2.2));
    color.rgb = mix(color.rgb, vec3(color.r), is_single_channel);

    color += vec3(.2);
    color *= uColor;

    fColor = vec4(color, 1);
    fPosition = vec4(vs_in.position,1);
    fNormal = vec4(normalize(vs_in.normal), 1);
    fEmission = vec4(uEmission ,1);
}

