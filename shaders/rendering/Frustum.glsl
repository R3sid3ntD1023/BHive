#type vertex
#version 460 core

uint indices[24] = uint[](

    //near rect
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    //far rect
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    //connecting edges
    0, 4,
    1, 5,
    2, 6,
    3, 7
);

layout(std140, set = 0, binding = 0) uniform Camera
{
    mat4 u_Proj;
    mat4 u_View;
} u_Cam;

layout(std140, set = 1, binding = 0) uniform FrustumPoints
{
    vec4 points[8]; //world space
} u_Frustum;


layout(location = 0) out vec3 v_Color;

void main()
{
    uint index = indices[gl_VertexIndex];
    vec3 worldPos = u_Frustum.points[index].xyz;
    gl_Position = u_Cam.u_Proj * u_Cam.u_View * vec4(worldPos, 1.0);

    if(index <= 3)
        v_Color = vec3(0.0, 1.0, 0.0f); //near
    else
        v_Color = vec3(1.0, 0.0, 0.0);//far
}

#type fragment

#version 460 core

layout(location = 0) in vec3 v_Color;
layout(location = 0) out vec4 o_Color;

void main()
{
    o_Color = vec4(v_Color, 1.0);
}