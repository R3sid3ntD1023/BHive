#type vertex
#version 460 core
#extension GL_ARB_shading_language_include : require

#include <Skinning.glsl>

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiNormal;
layout(location = 5) in vec4 vColor;
layout(location = 6) in ivec4 vBoneIds;
layout(location = 7) in vec4 vWeights;

layout(std140, binding = 0) uniform ObjectBuffer
{
	mat4 u_projection;
	mat4 u_view;
	mat4 u_model;
};

layout(location = 1) out struct vertex_output
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec4 color;
	vec3 camera_pos;
	mat3 TBN;
} vs_out;



void main()
{
	mat4 bone_transform = Skinning(vWeights, vBoneIds);

	gl_Position = u_projection * u_view * u_model * bone_transform * vec4(vPosition, 1.0);

	vs_out.position = vec3(u_model * bone_transform * vec4(vPosition, 1.0));
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;
	vs_out.camera_pos = inverse(u_view)[3].xyz;

	mat3 normal_matrix = mat3(transpose(inverse(u_model * bone_transform)));
	vec3 T = normalize(normal_matrix * vTangent);
	vec3 N = normalize(normal_matrix * vNormal);
	vec3 B = normalize(normal_matrix * vBiNormal);

	vs_out.normal = N;
	vs_out.TBN = mat3(T, B, N);
}

#type fragment
#version 460 core
#extension GL_ARB_shading_language_include : require

#include <Core.glsl>
#include <Lighting.glsl>
#include <BDRFFunctions.glsl>


layout(location = 1) in struct vertex_output
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec4 color;
	vec3 camera_pos;
	mat3 TBN;
} vs_in;


struct BDRFMaterial
{
	vec4 Albedo;
	float Metallic;
	float Roughness;
	float Opacity;
	int DiaElectric;
	vec3 Emission;	
	vec2 Tiling;
	float DepthScale;
	int flags;
};

layout(location = 1) uniform BDRFMaterial u_material = BDRFMaterial(vec4(.5), 0.f, 1.f, 1.f, 1, vec3(0), vec2(1,1), 1.0, 0);
layout(location = 16) uniform uint u_global_flags;

layout(binding = 0) uniform samplerCube u_prefilter_map;
layout(binding = 1) uniform samplerCube u_irradiance_map;
layout(binding = 2) uniform sampler2D u_brdf_lut;
layout(binding = 3) uniform sampler2DArrayShadow u_shadow_map;
layout(binding = 4) uniform sampler2DArrayShadow u_shadow_spot_map;
layout(binding = 5) uniform samplerCubeArrayShadow u_shadow_point_map;

layout(binding = 6) uniform sampler2D u_albedo_map;
layout(binding = 7) uniform sampler2D u_metallic_map;
layout(binding = 8) uniform sampler2D u_roughness_map;
layout(binding = 9) uniform sampler2D u_normal_map;
layout(binding = 10) uniform sampler2D u_emission_map;
layout(binding = 11) uniform sampler2D u_opacity_map;
layout(binding = 12) uniform sampler2D u_depth_map;
layout(binding = 13) uniform sampler2D u_metallic_roughness_map;


#define SHADOW_MAP_BINDING 7



vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float scale, in sampler2D depth);

layout(location = 0) out vec4 fs_out;

void main()
{
	vec2 coords = vs_in.texcoord;
	vec3 P = vs_in.position;
	vec3 N = normalize(vs_in.normal);
	vec3 V = normalize(vs_in.camera_pos - P);

	bool vertex_colors			= (u_material.flags & (1 << 0)) != 0;
	bool alpha_is_transparency	= (u_material.flags & (1 << 1)) != 0;
	bool metallic_roughness		= (u_material.flags & (1 << 2)) != 0;
	bool normal_map				= (u_material.flags & (1 << 3)) != 0;
	bool depth_map				= (u_material.flags & (1 << 4)) != 0;
	bool recieve_shadows		= (u_material.flags & (1 << 5)) != 0;
	bool render_shadows			= ((u_global_flags | (1 << 0)) != 0) && recieve_shadows;
	
	if(depth_map)
	{
		vec3 view = normalize(vs_in.camera_pos - P);
		coords = ParallaxMapping(coords, view, u_material.DepthScale, u_depth_map);
		if(coords.x > 1.0 || coords.y > 1.0 || coords.x < 0.0 || coords.y < 0.0)
    		 discard;
	} 

	coords *= u_material.Tiling;

	vec3 albedo = u_material.Albedo.rgb * pow(texture(u_albedo_map, coords).rgb, vec3(2.2)) ;
	float roughness = u_material.Roughness * texture(u_metallic_map, coords).r;
	float metallic = u_material.Metallic * texture(u_roughness_map, coords).r;
	vec3 emission = u_material.Emission * texture(u_emission_map, coords).rgb;
	float opacity = u_material.Opacity * texture(u_opacity_map, coords).r ;

	if(metallic_roughness)
	{
		roughness = u_material.Roughness * texture(u_metallic_roughness_map, coords).g;
		metallic = u_material.Metallic * texture(u_metallic_roughness_map, coords).b;
	}

	if(alpha_is_transparency)
	{
		opacity = u_material.Opacity * texture(u_albedo_map, coords).a; 
	}


	roughness = max(roughness, 0.0);
	metallic = max(metallic, 0.0);

	bool is_dia_electric = u_material.DiaElectric == 1;

	vec3 F0 = is_dia_electric ? vec3(0.04) : albedo;
	F0 =  mix(F0, albedo, metallic);

	if(vertex_colors)
	{
		fs_out = vs_in.color;
		return;
	}


	if(normal_map)
	{
		N = CalculateTangentSpaceNormal(N, vs_in.TBN, texture(u_normal_map, coords));
	}

	

	vec3 Lo = vec3(0.0);
	int j = 0;
	int k = 0;
	int s = 0;
	for(int i = 0; i < u_NumLights; i++)
	{
		Light light = lights[i];
		uint type = light.type;

		switch(type)
		{
			case 0:
			{
			
				vec3 l = CalculateDirectionalLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);
				if(render_shadows)
				{
					float shadow = DirLightShadow(k++, P, u_shadow_map);
					l *= shadow;
				}
				Lo += l;
				break;
			}
			case 1:
			{
				
				vec3 L = light.position - P;
				vec3 l = CalculatePointLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);

				if(render_shadows)
				{
					float shadow = PointLightShadow(j++, P, normalize(L), L, vec2(.1f, 50.f), u_shadow_point_map);
					l *= shadow;
				}
				Lo += l;
				break;
			}
			case 2:
			{
				vec3 l = CalculateSpotLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);
				if(render_shadows)
				{
					float shadow = SpotLightShadow(s++, P, u_shadow_spot_map);
					l *= shadow;
				}

				Lo += l;
				
				break;
			}
			default:
				break;
		}
		
	}

	vec3 environment = EnvironmentLighting(F0, N , V, albedo.rgb, metallic, roughness, u_irradiance_map, u_prefilter_map ,u_brdf_lut);

	fs_out = vec4(Lo + environment + emission, opacity);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float scale, in sampler2D depth)
{
	const float min_layers = 8.0;
	const float max_layers = 32.0;
	float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0, 0, 1), viewDir)));

	float layerDepth = 1.0 / num_layers;

	float current_layer_depth = 0.0;

	vec2 p = viewDir.xy / viewDir.z * scale;
	vec2 dcoords = p / num_layers;

	vec2 current_coords = texCoords;
	float current_depth_map_value = texture(depth, current_coords).r;

	while(current_layer_depth < current_depth_map_value)
	{
		current_coords -= dcoords;
		current_depth_map_value = texture(depth, current_coords).r;

		current_layer_depth += layerDepth;
	}

	vec2 prevTexCoords = current_coords + dcoords;

	float after_depth = current_depth_map_value - current_layer_depth;
	float before_depth = texture(depth, prevTexCoords).r - current_layer_depth + layerDepth;

	float weight = after_depth / (after_depth - before_depth);


	return prevTexCoords * weight + current_coords * (1.0 - weight);
}