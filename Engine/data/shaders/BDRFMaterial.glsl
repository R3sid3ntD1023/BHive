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
	vec2 u_near_far;
};

struct PerObjectData
{
	mat4 WorldMatrix;
};


layout(std430, binding = 1) restrict readonly buffer PerObjectSSBO
{
	PerObjectData object[];
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
	mat4 object_transform = object[gl_DrawID].WorldMatrix * bone_transform;

	gl_Position = u_projection * u_view * object_transform * vec4(vPosition, 1.0);

	vs_out.position = vec3(object_transform * vec4(vPosition, 1.0));
	vs_out.texcoord = vTexCoord;
	vs_out.color = vColor;
	vs_out.camera_pos = inverse(u_view)[3].xyz;

	// Calculate the TBN matrix

	mat3 normal_matrix = mat3(transpose(inverse(object_transform)));
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
	vec3 Emission;	
	vec2 Tiling;
	float Metallic;
	float Roughness;
	float Opacity;
	float DepthScale;
	int Flags;
};

#ifdef VULKAN
	layout(push_constant) uniform PushConstants
	{
		BDRFMaterial u_material;
		uint u_global_flags;
	} constants;

#else

	layout(location = 0) uniform struct PushConstants
	{
		BDRFMaterial u_material;
		uint u_global_flags;
	} constants;

#endif

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

	bool vertex_colors			= (constants.u_material.Flags & (1 << 0)) != 0;
	bool alpha_is_transparency	= (constants.u_material.Flags & (1 << 1)) != 0;
	bool metallic_roughness		= (constants.u_material.Flags & (1 << 2)) != 0;
	bool normal_map				= (constants.u_material.Flags & (1 << 3)) != 0;
	bool depth_map				= (constants.u_material.Flags & (1 << 4)) != 0;
	bool recieve_shadows		= (constants.u_material.Flags & (1 << 6)) != 0;
	bool dielectric				= (constants.u_material.Flags & (1 << 10)) != 0;
	bool render_shadows			= ((constants.u_global_flags & (1 << 0)) == 0) && recieve_shadows;
	
	if(depth_map)
	{
		vec3 view = normalize(vs_in.camera_pos - P);
		coords = ParallaxMapping(coords, view, constants.u_material.DepthScale, u_depth_map);
		if(coords.x > 1.0 || coords.y > 1.0 || coords.x < 0.0 || coords.y < 0.0)
    		 discard;
	} 

	coords *= constants.u_material.Tiling;

	vec3 albedo =		constants.u_material.Albedo.rgb * pow(texture(u_albedo_map, coords).rgb, vec3(2.2)) ;
	float roughness =	constants.u_material.Roughness * texture(u_metallic_map, coords).r;
	float metallic =	constants.u_material.Metallic * texture(u_roughness_map, coords).r;
	vec3 emission =		constants.u_material.Emission * texture(u_emission_map, coords).rgb;
	float opacity =		constants.u_material.Opacity * texture(u_opacity_map, coords).r ;

	if(metallic_roughness)
	{
		roughness = constants.u_material.Roughness * texture(u_metallic_roughness_map, coords).g;
		metallic =	constants.u_material.Metallic * texture(u_metallic_roughness_map, coords).b;
	}

	if(alpha_is_transparency)
	{
		opacity = constants.u_material.Opacity * texture(u_albedo_map, coords).a; 
	}


	roughness = max(roughness, 0.0);
	metallic = max(metallic, 0.0);

	vec3 F0 = dielectric ? vec3(0.04) : albedo;
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
	for(int i = 0; i < uNumLights; i++)
	{
		Light light = uLights[i];
		uint type = light.type;

		switch(type)
		{
			case 0:
			{
			
				vec3 l = CalculateDirectionalLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);
				if(render_shadows)
				{
					//float shadow = DirLightShadow(k++, P, u_shadow_map);
					//l *= shadow;
				}
				Lo += l;
				break;
			}
			case 1:
			{
				
				
				vec3 l = CalculatePointLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);

				if(render_shadows)
				{
					vec3 L = light.position - P;
					//float shadow = PointLightShadow(j++, P, normalize(L), L, vec2(.1f, 50.f), u_shadow_point_map);
					//l *= shadow;
				}
				Lo += l;
				break;
			}
			case 2:
			{
				vec3 l = CalculateSpotLightBDRF(F0, P, N, V, light, albedo, metallic, roughness);
				if(render_shadows)
				{
					//float shadow = SpotLightShadow(s++, P, u_shadow_spot_map);
					//l *= shadow;
				}

				Lo += l;
				
				break;
			}
			default:
				break;
		}
		
	}

	//vec3 environment = vec3(0.0);
	vec3 environment = EnvironmentLighting(F0, N , V, albedo.rgb, metallic, roughness, u_irradiance_map, u_prefilter_map ,u_brdf_lut);

	fs_out = vec4(Lo + environment + emission, opacity);
	//fs_out = vec4(1, 0, 0, 1); // For testing purposes, remove this line in production)
	//fs_out.rgb = albedo; // Gamma correction
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