static const char *line_vert = R"(
    #version 460  core
    #extension GL_NV_uniform_buffer_std430_layout : require

    layout(location = 0) in vec3 vPosition;
    layout(location = 1) in vec4 vColor;

    layout(std430, binding = 0) uniform Camera
    {
        mat4 uProjection;
        mat4 uView;
    };

    layout(location = 0) out struct VS_OUT
    {
        vec4 color;
    } vs_out;
        
    void main()
    {
        gl_Position = uProjection * uView * vec4(vPosition, 1.0);
        vs_out = VS_OUT(vColor);
    }
)";

static const char *line_frag = R"(
    #version 460 core

    layout(location = 0) in struct VS_OUT
    {
        vec4 color;
    } vs_in;

    layout(location = 0) out vec4 fColor;

    void main()
    {
        fColor = vs_in.color;
    }
)";