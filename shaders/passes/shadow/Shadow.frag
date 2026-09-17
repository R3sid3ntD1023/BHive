layout(set =0, binding = 9) uniform sampler2DArrayShadow u_shadow_map;
layout(set =0, binding = 10) uniform samplerCubeArrayShadow u_shadow_point_map;
layout(set =0, binding = 11) uniform sampler2DArrayShadow u_shadow_spot_map;
#define USE_SHADOWMAPS