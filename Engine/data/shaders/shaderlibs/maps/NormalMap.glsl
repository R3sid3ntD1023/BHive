#ifdef USE_NORMAL_MAP
bool has_normal_map = (constants.Flags & HAS_NORMAL_MAP) != 0;
if(has_normal_map)
{
	vec4 normal_map = texture(NormalMap, texCoord);
	normal = CalculateTangentSpaceNormal(normal, vs_in.TBN, normal_map);
}
#endif