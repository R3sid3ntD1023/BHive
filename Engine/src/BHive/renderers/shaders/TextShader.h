static const char *text_vert = R"(
   #version 460 core

    #extension GL_NV_uniform_buffer_std430_layout : require

    layout(location = 0) in vec4 vPos;
    layout(location = 1) in vec2 vTexCoord;
    layout(location = 2) in vec4 vColor;
    layout(location = 3) in int vTexture;
    layout(location = 4) in vec2 vThickness;
    layout(location = 5) in vec2 vOutline;
    layout(location = 6) in vec4 vOutlineColor;

    layout(std430 , binding = 0) uniform CameraBuffer
    {
        mat4 uProjection;
        mat4 uView;
    };

    layout(location = 0) out flat uint vs_texture;
    layout(location = 1) out struct VS_OUT
    {
        vec2 texCoord;
        vec4 color;
        vec2 thickness;
        vec2 outline;
        vec4 outlineColor;
    } vs_out;

    void main()
    {
        gl_Position = uProjection * uView * vPos;
        vs_out.texCoord = vTexCoord;
        vs_texture = vTexture;
        vs_out.color = vColor;
        vs_out.thickness = vThickness;
        vs_out.outline = vOutline;
        vs_out.outlineColor = vOutlineColor;
    }
)";

static const char *text_frag = R"(
    #version 460 core

    layout(location = 0) in flat uint vs_texture;
    layout(location = 1) in struct VS_OUT
    {
        vec2 texCoord;
        vec4 color;
        vec2 thickness;
        vec2 outline;
        vec4 outlineColor;
    } vs_in;

    layout(binding = 0) uniform sampler2D uTextures[32];

    layout(location = 0) out vec4 fColor;

    void main()
    {
        float thickness = vs_in.thickness.x;
        float smoothness = vs_in.thickness.y;
        float outline_thickness = vs_in.outline.x;
        float outline_smoothness = vs_in.outline.y;
        vec4 outline_color = vs_in.outlineColor;

       
        float a = texture(uTextures[vs_texture], vs_in.texCoord).r;
        float outline = smoothstep(outline_thickness - outline_smoothness, outline_thickness + outline_smoothness, a);
        a = smoothstep(1.0 - thickness - smoothness, 1.0 - thickness + smoothness, a);

        vec4 color = mix(vs_in.color, outline_color, 1.0 - outline);

        fColor = vec4(a) * color;
    }
)";