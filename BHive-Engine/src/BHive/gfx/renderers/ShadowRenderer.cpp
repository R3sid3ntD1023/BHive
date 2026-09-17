#include "ShadowRenderer.h"
#include "RenderBatch.h"
#include "Renderer.h"
#include "SceneRenderer.h"
#include "core/math/Frustum.h"
#include "gfx/ShaderManager.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/shader/Shader.h"

#define SHADOW_SSBO_BINDING 5
#define DIRECTIONAL_SHADOWMAP_SIZE 1024
#define POINT_SHADOWMAP_SIZE 1024
#define SPOT_SHADOWMAP_SIZE 512

namespace BHive
{
	struct LightDirections
	{
		glm::vec3 normal;
		glm::vec3 up;
	};

	static LightDirections point_directions[] = {
		{{1, 0, 0}, {0, -1, 0}},
		{{-1, 0, 0}, {0, -1, 0}},
		{{0, 1, 0}, {0, 0, 1}},
		{{0, -1, 0}, {0, 0, -1}},
		{{0, 0, 1}, {0, -1, 0}},
		{{0, 0, -1}, {0, -1, 0}},
	};

	struct FShadowCubeSSBO
	{
		glm::mat4 ShadowViewProjections[6];
		alignas(16) glm::vec2 ShadowNearFar;
	};

	struct FShadowData
	{
		glm::uvec4 NumShadowMaps = {0, 0, 0, 0}; // {Dir, Point, Spot}
		std::array<glm::mat4, ShadowRenderer::sMaxLights> DirProjections = {};
		std::array<FShadowCubeSSBO, ShadowRenderer::sMaxLights> PointShadowInfos = {};
		std::array<glm::mat4, ShadowRenderer::sMaxLights> SpotProjections = {};
	};

	// 0 = dir, 1 = point, 2 = spot
	struct FShadowPasses
	{
		std::array<MaterialPtr, 3> ShadowMats;
		std::array<FramebufferPtr, 3> FBOs;
	};

	struct FShadowRenderData
	{
		FShadowPasses ShadowPasses;
		BufferPtr ShadowBuffer;
		FShadowData ShadowData;
	};

	void ShadowRenderer::Init(uint32_t cascaded_levels)
	{
		mShadowRenderData = CreateRef<FShadowRenderData>();

		FramebufferSpecification dir_shadow_fbo_spec{.Size = {DIRECTIONAL_SHADOWMAP_SIZE, DIRECTIONAL_SHADOWMAP_SIZE}, .Depth = sMaxLights};
		FramebufferSpecification spot_shadow_fbo_spec{.Size = {SPOT_SHADOWMAP_SIZE, SPOT_SHADOWMAP_SIZE}, .Depth = sMaxLights};
		FramebufferSpecification point_shadow_fbo_spec{.Size = {POINT_SHADOWMAP_SIZE, POINT_SHADOWMAP_SIZE}, .Depth = sMaxLights};

		FTextureCreateInfo shadow_texture_specs{
			.Format = EFormat::DEPTH_COMPONENT_32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE, .CompareMode = ECompareMode::COMPARE_REF_TO_TEXTURE, .CompareOp = ECompareOp::LessOrEqual
		};

		FFramebufferTexture shadowTex;
		shadowTex.CreateInfo = shadow_texture_specs;
		shadowTex.Type = ETextureType::TEXTURE_2D_ARRAY;

		dir_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);
		spot_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);

		shadowTex.Type = ETextureType::TEXTURE_CUBE_MAP_ARRAY;
		shadowTex.CreateInfo.ArrayLayers = sMaxLights;
		point_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);

		auto &shadow_passes = mShadowRenderData->ShadowPasses;
		// shadow_passes.FBOs[0] = FramebufferFactory::Create(dir_shadow_fbo_spec);
		shadow_passes.FBOs[1] = FramebufferFactory::Create(point_shadow_fbo_spec);
		// shadow_passes.FBOs[2] = FramebufferFactory::Create(spot_shadow_fbo_spec);
		// shadow_passes.ShadowMats[0] = MaterialFactory::Create("ShadowDirectionalLight.glsl");
		shadow_passes.ShadowMats[1] = MaterialFactory::Create("ShadowPointLight.glsl");
		// shadow_passes.ShadowMats[2] = MaterialFactory::Create("ShadowSpotLight.glsl");

		mShadowRenderData->ShadowBuffer = BufferFactory::Create(sizeof(FShadowData), EBufferType::StorageBuffer);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Blend.Enabled = false;
		state.Depth.DepthWrite = true;
		state.Depth.DepthTest = true;
		state.Depth.DepthBias = true;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;
		state.Raster.CullEnabled = true;
		state.Raster.CullMode = ECullMode::Front;
		mPipeline = PipelineFactory::Create(state);
	}

	void ShadowRenderer::BeginRecording()
	{
		mShadowRenderData->ShadowData.NumShadowMaps = {0, 0, 0, 0};
	}

	void ShadowRenderer::EndRecording(FPass &pass, SceneRenderer *sceneRenderer)
	{

		pass.BeginPhase("Update Shadow Data", EPhaseType::Transfer);
		pass.Emplace<CmdSetBufferData>()(mShadowRenderData->ShadowBuffer, &mShadowRenderData->ShadowData, sizeof(FShadowData));
		pass.EndPhase();

		const auto &num_shadow_maps = glm::compAdd(mShadowRenderData->ShadowData.NumShadowMaps);
		if (num_shadow_maps == 0)
			return;

		auto draw_meshes = [](FPass &_pass, BufferPtr indirect, const RenderBatch &batch, MaterialPtr material)
		{
			for (auto &[vao, matMap] : batch.MaterialBatches)
			{
				for (auto &[mat, batch] : matMap)
				{
					_pass.Emplace<CmdBindMaterial>()(material.As<Material>());
					uint32_t offset = batch.FirstCommand * sizeof(MultiDrawIndirectCommand);

					_pass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, indirect, vao, batch.CommandCount, sizeof(MultiDrawIndirectCommand), offset);
				}
			}
		};

		if (mShadowRenderData->ShadowData.NumShadowMaps.x > 0)
		{
			// mShadowRenderData->ShadowPasses.FBOs[0]->Bind();

			// RenderCommand::Clear(Buffer_Depth);

			// draw_meshes(mShadowRenderData->ShadowPasses.Shaders[0]);

			// mShadowRenderData->ShadowPasses.FBOs[0]->UnBind();
		}

		if (mShadowRenderData->ShadowData.NumShadowMaps.y > 0)
		{
			auto fbo = mShadowRenderData->ShadowPasses.FBOs[1];
			auto buffer = mShadowRenderData->ShadowBuffer;
			auto material = mShadowRenderData->ShadowPasses.ShadowMats[1];
			auto indirect = sceneRenderer->mIndirectDrawBuffer[0];
			auto visibility = sceneRenderer->mVisibleBuffer[0];
			auto instance = sceneRenderer->mInstanceDataBuffer[0];
			auto &batch = *sceneRenderer->mRenderBatches[0];

			for (uint32_t face = 0; face < 6; ++face)
			{
				material.As<Material>()->SetParam("LightIndex", MaterialParam{0});
				material.As<Material>()->SetParam("LightFace", MaterialParam{face});
				pass.BeginPhase("Render Point Shadow Maps", EPhaseType::Graphics);
				pass.UseFramebuffer(fbo, ImageSubresourceRange{.BaseArrayLayer = CubeFaceLayer(0, face)});
				pass.UseBuffer(buffer, EBufferUsage::StorageRead);
				pass.UseBuffer(indirect, EBufferUsage::IndirectRead);
				pass.UseBuffer(visibility, EBufferUsage::StorageRead);
				pass.UseBuffer(instance, EBufferUsage::StorageRead);
				pass.BindResourceSet(sceneRenderer->mSceneSets.GlobalSet);
				pass.BindResourceSet(sceneRenderer->mSceneSets.OpaqueObjectSet);
				pass.Emplace<CmdBindPipeline>()(mPipeline);
				draw_meshes(pass, indirect, batch, material);

				pass.EndPhase();
			}
		}

		if (mShadowRenderData->ShadowData.NumShadowMaps.z > 0)
		{
			// mShadowRenderData->ShadowPasses.FBOs[2]->Bind();

			// RenderCommand::Clear(Buffer_Depth);

			// draw_meshes(mShadowRenderData->ShadowPasses.Shaders[2]);

			// mShadowRenderData->ShadowPasses.Shaders[2]->UnBind();
		}
	}

	void ShadowRenderer::SubmitDirectionalLight(const FShadowCascadedCreateInfo &info)
	{

		auto frustum = Frustum(info.CameraProj, info.InverseCameraView);

		const auto light_view = glm::lookAt({}, info.LightDirection, {0, 1, 0});

		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::lowest();

		for (const auto &v : frustum.GetPoints())
		{
			const auto trf = light_view * v;
			min_x = std::min(min_x, trf.x);
			max_x = std::max(max_x, trf.x);
			min_y = std::min(min_y, trf.y);
			max_y = std::max(max_y, trf.y);
			min_z = std::min(min_z, trf.z);
			max_z = std::max(max_z, trf.z);
		}

		constexpr float z_multi = 10.0f;
		if (min_z < 0)
		{
			min_z *= z_multi;
		}
		else
		{
			min_z /= z_multi;
		}
		if (max_z < 0)
		{
			max_z /= z_multi;
		}
		else
		{
			max_z *= z_multi;
		}

		auto &shadow_data = mShadowRenderData->ShadowData;
		auto k = shadow_data.NumShadowMaps.x % sMaxLights;
		auto projection = glm::ortho<float>(min_x, max_x, min_y, max_y, min_z, max_z);
		shadow_data.DirProjections[k] = projection * light_view;
		shadow_data.NumShadowMaps.x++;
	}

	void ShadowRenderer::SubmitSpotLight(const FShadowFrustumCreateInfo &info)
	{
		auto view = glm::lookAt(info.LightPosition, info.LightPosition + info.LightDirection, {0, 1, 0});
		auto proj = glm::perspective<float>(glm::radians(info.LightAngleNearFar.x), 1.f, info.LightAngleNearFar.y, info.LightAngleNearFar.z);

		auto &shadow_data = mShadowRenderData->ShadowData;
		auto k = shadow_data.NumShadowMaps.z % sMaxLights;
		shadow_data.SpotProjections[k] = proj * view;
		shadow_data.NumShadowMaps.z++;
	}

	void ShadowRenderer::SubmitPointLight(const FShadowCubeCreateInfo &info)
	{
		auto &shadow_data = mShadowRenderData->ShadowData;

		auto proj = glm::perspective(glm::radians(90.0f), 1.f, info.LightNearFar.x, info.LightNearFar.y);
		auto i = (shadow_data.NumShadowMaps.y % sMaxLights);

		for (int j = 0; j < 6; j++)
		{
			auto view = glm::lookAt(info.LightPosition, info.LightPosition + point_directions[j].normal, point_directions[j].up);
			shadow_data.PointShadowInfos[i].ShadowViewProjections[j] = proj * view;
		}

		shadow_data.PointShadowInfos[i].ShadowNearFar = info.LightNearFar;
		shadow_data.NumShadowMaps.y++;
	}

	BufferPtr ShadowRenderer::GetBuffer()
	{
		return mShadowRenderData->ShadowBuffer;
	}

	TexturePtr ShadowRenderer::GetDirShadowMap()
	{
		return TexturePtr();
	}

	TexturePtr ShadowRenderer::GetPointShadowMap()
	{
		return mShadowRenderData->ShadowPasses.FBOs[1].As<Framebuffer>()->GetDepthAttachment();
	}

	TexturePtr ShadowRenderer::GetSpotShadowMap()
	{
		return TexturePtr();
	}

} // namespace BHive