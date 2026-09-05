#include <ApplyLighting.glsl>

layout(location = 0) out vec4 fs_out;

void main()
{
	Material mat = GetMaterial(vs_in);

	ReflectedLight reflected = ReflectedLight(vec3(0), vec3(0), vec3(0), vec3(0));

	ApplyLighting(vs_in.Position, mat.Normal, normalize(vs_in.CameraPosition - vs_in.Position), mat, reflected);

	vec3 outColor = reflected.DirectDiffuse + reflected.IndirectDiffuse + reflected.DirectSpecular + reflected.IndirectSpecular;

#ifdef HAS_EMISSION
	outColor += mat.Emission;
#endif

#ifdef HAS_OPACITY
	#define OPACITY mat.Opacity
#else
	#define OPACITY 1.0f
#endif

	//outColor = vs_in.DebugColor;

	fs_out = vec4(outColor, OPACITY);
}