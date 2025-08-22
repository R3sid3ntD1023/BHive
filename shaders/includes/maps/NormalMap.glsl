#ifdef USE_NORMAL_MAP
if(HasNormalMap)
{
	vec4 normal_map = texture(NormalMap, texCoord);
	normal = CalculateTangentSpaceNormal(normal, vs_in.TBN, normal_map);
}
#endif