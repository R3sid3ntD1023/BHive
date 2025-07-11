#include "PMREMGenerator.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/textures/TextureCube.h"
#include "gfx/textures/Texture2D.h"
#include "mesh/primitives/Cube.h"
#include <glad/glad.h>
#include "gfx/ShaderManager.h"
#include "gfx/UniformBuffer.h"
#include "Renderer.h"

#define ENVIRONMENT_MAP_SIZE 512
#define PREFILTER_MAP_SIZE 128
#define PREFILTER_MIP_LEVELS 5
#define PREFILTER_WORK_GROUP_SIZE 8
#define MAX_PREFILTER_SAMPLES 64
#define IRRANDIANCE_CUBEMAP_SIZE 32
#define BRDF_LUT_SIZE 512
#define BRDF_WORK_GROUP_SIZE 8

namespace BHive
{
	void PMREMGenerator::Initialize()
	{
		if (mInitialized)
			return;

		mEnvironmentCapture = CreateRef<RenderTargetCube>(ENVIRONMENT_MAP_SIZE, EFormat::RGBA32F);
		mIrradianceCapture = CreateRef<RenderTargetCube>(IRRANDIANCE_CUBEMAP_SIZE, EFormat::RGBA32F);

		FTextureSpecification pre_filter_specification;
		pre_filter_specification.InternalFormat = EFormat::RGBA16F;
		pre_filter_specification.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		pre_filter_specification.MinFilter = EMinFilter::MIPMAP_LINEAR;
		pre_filter_specification.MagFilter = EMagFilter::LINEAR;
		pre_filter_specification.Levels = PREFILTER_MIP_LEVELS;

		mPreFilteredEnvironmentTexture = CreateRef<TextureCube>(PREFILTER_MAP_SIZE, pre_filter_specification);

		mBRDFLUTTexture = CreateRef<Texture2D>(
			BRDF_LUT_SIZE, BRDF_LUT_SIZE,
			FTextureSpecification{
				.InternalFormat = EFormat::RG16F,
				.WrapMode = EWrapMode::CLAMP_TO_EDGE,
				.MinFilter = EMinFilter::NEAREST,
				.MagFilter = EMagFilter::NEAREST,
			});

		mCube = CreateRef<PCube>(2.0f);

		mEquirectangularShader =
			ShaderManager::Get().Load("EquirectangularShader", GetCommonVertexShader(), GetEquirectangularShader());

		mIrradianceShader = ShaderManager::Get().Load("IrradianceShader", GetCommonVertexShader(), GetIrradianceShader());
		mPreFilterEnironmentShader =
			ShaderManager::Get().Load("PreFilterEnvironmentShader", GetPreFilterEnvironmentShader());
		mBRDFLUTShader = ShaderManager::Get().Load("BRDFLUTShader", GetBRDFLUTShader());

		mInitialized = true;
	}

	void PMREMGenerator::SetEnvironmentMap(const Ref<Texture> &texture)
	{
		mEnvironmentTexture = texture;
		CreateEnvironmentCubeMap();
		CreateIrradianceMap();
		CreateBRDFLUTMap();
		CreatePreFilteredEnvironmentMap();
	}

	const Ref<Texture> &PMREMGenerator::GetIrradianceTexture() const
	{
		return mIrradianceCapture->GetTargetTexture();
	}

	const Ref<Texture> &PMREMGenerator::GetPreFilteredEnvironmentTetxure() const
	{
		return mPreFilteredEnvironmentTexture;
	}

	const Ref<Texture> &PMREMGenerator::GetBDRFLUT() const
	{
		return mBRDFLUTTexture;
	}

	void PMREMGenerator::CreateEnvironmentCubeMap()
	{
		for (int i = 0; i < 6; i++)
		{
			mEnvironmentCapture->Bind(i);

			RenderCommand::CullFront();
			RenderCommand::Clear();

			mEquirectangularShader->Bind();
			mEnvironmentTexture->Bind(0);

			RenderCube(i);

			mEquirectangularShader->UnBind();
			mEnvironmentCapture->UnBind();

			RenderCommand::CullBack();
		}

		mEnvironmentCapture->GetTargetTexture()->GenerateMipMaps();
	}

	void PMREMGenerator::CreateIrradianceMap()
	{

		for (int i = 0; i < 6; i++)
		{
			mIrradianceCapture->Bind(i);

			RenderCommand::CullFront();
			RenderCommand::Clear();

			mIrradianceShader->Bind();
			mEnvironmentCapture->GetTargetTexture()->Bind();

			RenderCube(i);

			mIrradianceShader->UnBind();
			mIrradianceCapture->UnBind();

			RenderCommand::CullBack();
		}
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		mPreFilterEnironmentShader->Bind();
		mEnvironmentCapture->GetTargetTexture()->Bind();

		int mip_level = (ENVIRONMENT_MAP_SIZE / PREFILTER_MAP_SIZE) - 1;
		for (int i = 0; i < PREFILTER_MIP_LEVELS; i++)
		{
			unsigned w = PREFILTER_MAP_SIZE * pow(0.5f, i);
			unsigned h = PREFILTER_MAP_SIZE * pow(0.5f, i);

			float roughness = (float)i / (float)(PREFILTER_MIP_LEVELS - 1);

			mPreFilterEnironmentShader->SetUniform("constants.u_roughness", roughness);
			mPreFilterEnironmentShader->SetUniform("constants.u_mip_level", mip_level);
			mPreFilterEnironmentShader->SetUniform("constants.u_width", w);
			mPreFilterEnironmentShader->SetUniform("constants.u_height", h);

			mPreFilteredEnvironmentTexture->BindAsImage(0, EImageAccess::WRITE, i);

			mPreFilterEnironmentShader->Dispatch(w / PREFILTER_WORK_GROUP_SIZE, h / PREFILTER_WORK_GROUP_SIZE, 6);
		}

		mPreFilterEnironmentShader->UnBind();
	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		mBRDFLUTShader->Bind();

		mBRDFLUTTexture->BindAsImage(0, EImageAccess::WRITE);
		mBRDFLUTShader->Dispatch(BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE);

		mBRDFLUTShader->UnBind();
	}

	void PMREMGenerator::RenderCube(uint32_t face)
	{
		Renderer::Begin();
		Renderer::SubmitCamera(mCubeCamera.GetProjection(), mCubeCamera.GetView({}, face));

		auto &submesh = mCube->GetSubMeshes()[0];
		RenderCommand::DrawElementsBaseVertex(
			EDrawMode::Triangles, *mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

		Renderer::End();
	}

	const std::string PMREMGenerator::GetCommonVertexShader() const
	{
		return R"(#version 460 core

		layout(location = 0) in vec3 vPosition;

		layout(location = 0) out struct vertex_out
		{
			vec3 position;
		} vs_out;

		layout(std140, binding = 0) uniform CameraBuffer
		{
			mat4 u_projection;
			mat4 u_view;
			vec2 u_near_far;
		};

		void main()
		{
			gl_Position = u_projection * u_view * vec4(vPosition, 1);
			vs_out.position = vPosition;
		})";
	}

	const std::string PMREMGenerator::GetEquirectangularShader() const
	{
		return "#version 460 core" + GetCommmonFunctions() + R"(

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
				color = ACES(color);

				fs_out = vec4(color, 1);
			})";
	}

	const std::string PMREMGenerator::GetIrradianceShader() const
	{
		return "#version 460 core" + GetCommmonFunctions() +
			   R"(
		layout(location = 0) in struct vertex_output
		{
			vec3 position;
		} vs_in;

		layout(binding = 0) uniform samplerCube environmentMap;

		layout(location = 0) out vec4 fs_out;

		void main()
		{
			vec3 normal = normalize(vs_in.position);

			vec3 irradiance = vec3(0);
			vec3 up = vec3(0, 1, 0);
			vec3 right = normalize(cross(up, normal));
			up = normalize(cross(normal, right));

			float samplesDelta = 0.025;
			float nrSamples = 0;
			for(float phi = 0; phi < 2.0 * PI; phi += samplesDelta)
			{
				for(float theta = 0; theta < 0.5 * PI; theta += samplesDelta)
				{
					vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
					vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

					irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
					nrSamples++;
				}
			}

			irradiance = PI * irradiance * (1.0 / float(nrSamples));

			fs_out = vec4(irradiance, 1);
		})";
	}

	const std::string PMREMGenerator::GetBRDFLUTShader() const
	{
		return "#version 460 core" + GetCommmonFunctions() + R"(

		#define LOCAL_SIZE 8

		layout(local_size_x =  LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;

		layout(rg16f, binding =  0) uniform restrict writeonly image2D brdfLutTexture;

		float GeomertySchlickGGX(float NDotV, float roughness)
		{
			float r = roughness;
			float k = (r * r) / 2.0;
			return NDotV / (NDotV * (1.0 - k) + k);
		}

		float GeomertySmith(vec3 N, vec3 V, vec3 L, float roughness)
		{
			float ndotv = max(dot(N, V), 0.0);
			float ndotl = max(dot(N, L), 0.0);
			float ggx2  = GeomertySchlickGGX(ndotv, roughness);
			float ggx1  = GeomertySchlickGGX(ndotl, roughness);

			return ggx1 * ggx2;
		}

		vec2 IntegratedBRDF(float NdotV, float roughness)
		{
			vec3 V;
			V.x = sqrt(1.0 - NdotV * NdotV);
			V.y = 0.0;
			V.z = NdotV;

			float A = 0.0;
			float B = 0.0;

			vec3 N = vec3(0 , 0, 1);

			const uint SAMPLE_COUNT  = 1024u;
			for(uint i = 0 ; i  <SAMPLE_COUNT; i++)
			{
				vec2 Xi = Hamemersley(i , SAMPLE_COUNT);
				vec3 H = ImportanceSampleGGX(Xi, N, roughness);
				vec3 L = normalize(2.0 * dot(V, H) * H - V);

				float NdotL = max(L.z, 0.0);
				float NdotH = max(H.z ,0.0);
				float VdotH = max(dot(V, H), 0.0);

				if(NdotL > 0.0)
				{
					float G = GeomertySmith(N, V, L, roughness);
					float G_Vis = (G * VdotH) / (NdotH * NdotV);
					float Fc = pow(1.0 - VdotH, 5.0);

					A += (1.0 - Fc) * G_Vis;
					B += Fc * G_Vis;
				}
			}

			A /= float(SAMPLE_COUNT);
			B /= float(SAMPLE_COUNT);
			return vec2(A, B);

		}

		

		void main()
		{
			vec2 texCoords = vec2(float(gl_GlobalInvocationID.x) , float(gl_GlobalInvocationID.y)) / float(512.0 - 1.0);

			vec2 integratedBRDF = IntegratedBRDF(max(texCoords.x, .0001), texCoords.y);

			imageStore(brdfLutTexture, ivec2(gl_GlobalInvocationID.xy), vec4(integratedBRDF, 0 , 0));
		}

		)";
	}

	const std::string PMREMGenerator::GetPreFilterEnvironmentShader() const
	{
		return "#version 460 core" + GetCommmonFunctions() +
			   R"(

		

		#define LOCAL_SIZE 8

		layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;

		layout(rgba16f, binding = 0) uniform restrict writeonly imageCube imgOutput;
		layout(binding = 0) uniform samplerCube environmentMap;

		#ifdef VULKAN
			layout(std140, push_constant) uniform PushConstants
			{
				float u_roughness;
				int u_mip_level;
				uint u_width;
				uint u_height;
			} constants;
		#else

			layout(location = 0) uniform struct PushConstants
			{
				float u_roughness;
				int u_mip_level;
				uint u_width;
				uint u_height;
			} constants;

		#endif

		void main()
		{
			vec3 N = CalculateDirection(gl_GlobalInvocationID.z, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, float(constants.u_width), float(constants.u_height));
			vec3 R = N;
			vec3 V = R;
			float roughness = constants.u_roughness;
			int current_mip_level = constants.u_mip_level;

			const uint SAMPLE_COUNT = 1024u;
			float weight = 0.0;
			vec3 prefilterColor = vec3(0);
			for(uint i = 0u; i < SAMPLE_COUNT; i++)
			{
				vec2 Xi = Hamemersley(i, SAMPLE_COUNT);
				vec3 H = ImportanceSampleGGX(Xi, N, roughness);
				vec3 L = normalize(2.0 * dot(V, H) * H - V);

				float NdotL = max(dot(N, L), 0.0);
				if(NdotL > 0.0)
				{
					float NdotH  = max(dot(N, H), 0.0);
					float HdotV  = max(dot(H, V), 0.0);

					float D = DisributionGXX(N, H, roughness);
					float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

					float resolution = 512.0;
					float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
					float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

					float mip_level = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
					prefilterColor += textureLod(environmentMap, L, current_mip_level + mip_level).rgb * NdotL;

					weight += NdotL;
				}
			}

			prefilterColor /= weight;
    

			imageStore(imgOutput, ivec3(gl_GlobalInvocationID), vec4(prefilterColor, 1));
		}

		)";
	}

	const std::string PMREMGenerator::GetCommmonFunctions() const
	{
		return R"(

		#define PI 3.14159265359
		#define POS_X 0
		#define NEG_X 1
		#define POS_Y 2
		#define NEG_Y 3
		#define POS_Z 4
		#define NEG_Z 5

		float DisributionGXX(vec3 N, vec3 H, float roughness)
		{
			float a2 = roughness * roughness * roughness * roughness;
			float ndoth = max(dot(N, H), 0.0);
			float ndoth2 = ndoth * ndoth;

			float denom = (ndoth2 * (a2 - 1.0) + 1.0);
			return a2 /  (PI * denom * denom) + 0.0001;

		}

		float RadicalInverse_VdC(uint bits)
		{
			bits = (bits << 16u) | (bits >> 16u);
			bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
			bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
			bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0x0F0F0F0Fu) >> 4u);
			bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
			return float(bits)  * 2.3283064365386963e-10;
		}

		vec2 Hamemersley(uint i, uint N)
		{
			return vec2(float(i)/float(N), RadicalInverse_VdC(i));
		}

		vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
		{
			float a = roughness * roughness;
			float phi = 2.0 * PI * Xi.x;
			float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
			float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

			vec3 H;
			H.x = cos(phi) * sinTheta;
			H.y = sin(phi) * sinTheta;
			H.z = cosTheta;

			vec3 up = abs(N.z) < 0.999 ? vec3(0 ,0 ,1) : vec3(1, 0, 0);
			vec3 tangent = normalize(cross(up, N));
			vec3 bitangent = cross(N, tangent);

			vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
			return normalize(sampleVec);
		}

		//https://github.com/diharaw/runtime-ibl/blob/master/src/shader/prefilter_cs.glsl

		float unlerp(float val, float max_val)
		{

			return (val + 0.5) / max_val;
		}

		vec3 CalculateDirection(uint face, uint face_x, uint face_y, float w, float h)
		{
			float s = unlerp(float(face_x), w) * 2.0 - 1.0;
			float t = unlerp(float(face_y), h) * 2.0 - 1.0;
			float x, y ,z;

			switch(face)
			{
				case POS_Z:
					x = s;
					y = -t;
					z = 1;
					break;
				case NEG_Z:
					x = -s;
					y = -t;
					z = -1;
					break;
				case NEG_X:
					x = -1;
					y = -t;
					z = s;
					break;
				case POS_X:
					x = 1;
					y = -t;
					z = -s;
					break;
				case POS_Y:
					x = s;
					y = 1;
					z = t;
					break;
				case NEG_Y:
					x = s;
					y = -1;
					z = -t;
					break;
				default:
				break;
			}

			vec3 d;
			float inv_len = 1.0 /sqrt(x * x + y * y + z * z);
			d.x = x * inv_len;
			d.y = y * inv_len;
			d.z = z * inv_len;

			return d;
		})";
	}
} // namespace BHive
