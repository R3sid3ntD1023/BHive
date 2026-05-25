#type vertex
#version 460 core

layout(location = 0) out vec2 v_UV;

const vec2 POS[3] = vec2[]
(
	vec2(-1.0, -1.0),
	vec2(3.0, -1.0),
	vec2(-1.0, 3.0)
);

const vec2 UVS[3] = vec2[]
(
	vec2(0.0, 0.0),
	vec2(2.0, 0.0),
	vec2(0.0, 2.0)
);

void main()
{
	gl_Position = vec4(POS[gl_VertexIndex], 0.0, 1.0);
	v_UV = UVS[gl_VertexIndex];
}

#type fragment
#version 460 core

layout(location = 0) in vec2 v_UV;
layout(location = 0) out vec4 o_Color;

layout(set = 1, binding = 0) uniform sampler2D u_Tex2D;
layout(set = 1, binding = 1) uniform samplerCube u_TexCube;

layout(push_constant) uniform PushConstants
{
	int u_Type; // 0 = 2D, 1 = Cube
	int u_Mip;
	int u_Face; //0..5 for cube
} PC;

vec3 FaceDir(int face, vec2 uv)
{
	uv = uv * 2.0 - 1.0;

	if(face == 0) return normalize(vec3(1.0, uv.y, -uv.x)); //+X
	if(face == 1) return normalize(vec3(-1.0, uv.y, uv.x));	//-X
	if(face == 2) return normalize(vec3(uv.x, 1.0, -uv.y));	//+Y
	if(face == 3) return normalize(vec3(uv.x, -1.0, uv.y));	//-Y
	if(face == 4) return normalize(vec3(uv.x, uv.y, 1.0));	//+Z
	return normalize(vec3(-uv.x, uv.y, -1.0)); //-Z
}

void main()
{
	float lod = float(PC.u_Mip);
	if(PC.u_Type == 0)
	{
		o_Color = textureLod(u_Tex2D, v_UV, lod);
	}
	else
	{
		vec3 dir = FaceDir(PC.u_Face, v_UV);
		vec3 c = textureLod(u_TexCube, dir, lod).rgb;
		o_Color = vec4(c, 1.0);
	}
}
