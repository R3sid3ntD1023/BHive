
vec3 normal = vs_in.normal;

#if defined(USE_NORMAL_MAP)

bool has_normal_map = (constants.u_Flags & HAS_NORMAL_MAP) != 0;
if(has_normal_map)
{
	vec4 normal_map = texture(u_NormalMap, vs_in.texcoord);
	normal = CalculateTangentSpaceNormal(vs_in.normal, vs_in.TBN, normal_map);
}
	
#endif