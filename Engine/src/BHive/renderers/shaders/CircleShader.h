static const char *circle_vert = R"(
    #version 460 core

    #extension GL_NV_uniform_buffer_std430_layout : require

    layout(location = 0) in vec4 vWorldPosition;
    layout(location = 1) in vec3 vLocalPosition;
    layout(location = 2) in vec4 vColor;
    layout(location = 3) in float vThickness;
    layout(location = 4) in float vFade;

    layout(std430, binding = 0) uniform Camera
    {
        mat4 uProjection;
        mat4 uView;
    };

    layout(location = 0) out struct VS_OUT
    {
        vec3 localPosition;
        vec4 color;
        float thickness;
        float fade;
    } vs_out;

    void main()
    {
        gl_Position = uProjection * uView * vWorldPosition;
        vs_out = VS_OUT(vLocalPosition, vColor, vThickness, vFade);
    }
)";
static const char *circle_frag = R"(
    #version 460 core

    layout(location = 0) in struct VS_OUT
    {
        vec3 localPosition;
        vec4 color;
        float thickness;
        float fade;
    } vs_in;

    layout(location = 0) out vec4 fColor;

    void main()
    {
        float dist = 1.0 - length(vs_in.localPosition);
        float circle = smoothstep(0.0, vs_in.fade, dist);
        circle *= smoothstep(vs_in.thickness + vs_in.fade, vs_in.thickness, dist);

        fColor = vs_in.color;
        fColor.a *= circle;
    }
)";