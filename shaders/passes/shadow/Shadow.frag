layout(binding = 9) uniform sampler2DArrayShadow u_shadow_map;
layout(binding = 10) uniform samplerCubeArrayShadow u_shadow_point_map;
layout(binding = 11) uniform sampler2DArrayShadow u_shadow_spot_map;
#define USE_SHADOWMAPS