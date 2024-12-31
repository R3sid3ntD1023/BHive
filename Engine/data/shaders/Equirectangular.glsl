#type vertex
#version 460 core

layout(location = 0) in vec3 vPosition;

void main()
{
    gl_Position = vec4(vPosition, 1);
}

#type geometry
#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(location = 0) uniform mat4 u_projection;
layout(location = 1) uniform mat4 u_views[6];

layout(location = 0) out struct vertex_out
{
    vec3 position;
} vs_out;


void main()
{
    for(int f = 0; f < 6; f++)
    {
        gl_Layer = f;
        for(int i = 0; i < gl_in.length(); i++)
        {
            vec4 pos = gl_in[i].gl_Position;
            vs_out.position = pos.xyz;
            gl_Position = u_projection * u_views[f] * pos;
            
            
            EmitVertex();
        }
            
        EndPrimitive();
    }
}

#type fragment
#version 460 core

layout(location = 0) in struct vertex_out
{
    vec3 position;
} vs_in;

layout(binding =  0) uniform sampler2D equirectangularMap;

layout(location =  0 ) out vec4 fs_out;

vec2 SampleSphericalMap(vec3 v)
{
    const vec2 invATan = vec2(0.1591, 0.3183);

    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invATan;
    uv += 0.5;
    return uv;
}

vec3 ACES(vec3 color) {
  const float a = 2.51f;
  const float b = 0.03f;
  const float c = 2.43f;
  const float d = 0.59f;
  const float e = 0.14f;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(vs_in.position));
    vec3 color = texture(equirectangularMap, uv).rgb;

   //color = color / (color + vec3(1.0));
   //color = pow(color, vec3(1.0/2.2));
   color = ACES(color);

    fs_out = vec4(color, 1);

}
