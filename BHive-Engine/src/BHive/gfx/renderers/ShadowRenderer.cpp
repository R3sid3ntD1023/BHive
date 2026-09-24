#include "ShadowRenderer.h"
#include "RenderBatch.h"
#include "Renderer.h"
#include "SceneRenderer.h"
#include "core/math/Frustum.h"
#include "gfx/ShaderManager.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/shader/Shader.h"

#define DIRECTIONAL_SHADOWMAP_SIZE 2048
#define POINT_SHADOWMAP_SIZE 256
#define SPOT_SHADOWMAP_SIZE 256

namespace BHive
{
	struct PointLightDirection
	{
		glm::vec3 normal;
		glm::vec3 up;
	};

	static const PointLightDirection point_directions[] = {
		{{1, 0, 0}, {0, -1, 0}},
		{{-1, 0, 0}, {0, -1, 0}},
		{{0, 1, 0}, {0, 0, 1}},
		{{0, -1, 0}, {0, 0, -1}},
		{{0, 0, 1}, {0, -1, 0}},
		{{0, 0, -1}, {0, -1, 0}},
	};

	void ShadowRenderer::Init()
	{
		FramebufferSpecification dir_shadow_fbo_spec{.Size = {DIRECTIONAL_SHADOWMAP_SIZE, DIRECTIONAL_SHADOWMAP_SIZE}};
		FramebufferSpecification spot_shadow_fbo_spec{.Size = {SPOT_SHADOWMAP_SIZE, SPOT_SHADOWMAP_SIZE}};
		FramebufferSpecification point_shadow_fbo_spec{.Size = {POINT_SHADOWMAP_SIZE, POINT_SHADOWMAP_SIZE}};

		FTextureCreateInfo shadow_texture_specs{
			.Format = EFormat::DEPTH_COMPONENT_32F,
			.WrapMode = EWrapMode::CLAMP_TO_BORDER,
			.BorderColor = EBorderColor::FLOAT_OPAQUE_WHITE,
			.CompareMode = ECompareMode::COMPARE_REF_TO_TEXTURE,
			.CompareOp = ECompareOp::LessOrEqual
		};

		shadow_texture_specs.ArrayLayers = sMaxLights * 4;
		dir_shadow_fbo_spec.Attachments.SetDepthAttachment(shadow_texture_specs, ETextureType::TEXTURE_2D_ARRAY);
		shadow_texture_specs.ArrayLayers = sMaxLights;
		spot_shadow_fbo_spec.Attachments.SetDepthAttachment(shadow_texture_specs, ETextureType::TEXTURE_2D_ARRAY);
		shadow_texture_specs.ArrayLayers = sMaxLights;
		point_shadow_fbo_spec.Attachments.SetDepthAttachment(shadow_texture_specs, ETextureType::TEXTURE_CUBE_MAP_ARRAY);

		mShadowFramebuffers[0] = FramebufferFactory::Create(dir_shadow_fbo_spec);
		mShadowFramebuffers[1] = FramebufferFactory::Create(point_shadow_fbo_spec);
		mShadowFramebuffers[2] = FramebufferFactory::Create(spot_shadow_fbo_spec);

		mShadowMaterials[0] = MaterialFactory::Create("ShadowDirectionalLight.glsl");
		mShadowMaterials[1] = MaterialFactory::Create("ShadowPointLight.glsl");
		mShadowMaterials[2] = MaterialFactory::Create("ShadowSpotLight.glsl");
		mCullingMaterial = MaterialFactory::Create("ShadowCulling.glsl");

		mShadowBuffer = BufferFactory::Create(sizeof(ShadowData), EBufferType::StorageBuffer);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.Blend.Enabled = false;
		state.Depth.DepthWrite = true;
		state.Depth.DepthTest = true;
		state.Depth.DepthBias = true;
		state.Depth.ConstantFactor = 0.5f;
		state.Depth.SlopeFactor = 1.0f;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;
		state.Raster.CullEnabled = true;
		state.Raster.CullMode = ECullMode::Front;
		mPipeline = PipelineFactory::Create(state);

		auto directionalState = state;
		directionalState.Depth.ConstantFactor = 0.25f;
		directionalState.Depth.SlopeFactor = 0.5f;
		mDirectionalPipeline = PipelineFactory::Create(directionalState);

		InitializeBuffers();
		InitializeSets();
	}

	void ShadowRenderer::BeginRecording()
	{
		mShadowData.NumShadowMaps = {0, 0, 0, 0};
	}

	void ShadowRenderer::SetLightBuffer(BufferPtr lightBuffer)
	{
		mGlobalSet.As<ResourceSet>()->SetBuffer(1, lightBuffer);
	}

	void ShadowRenderer::SetBoneBuffer(BufferPtr boneBuffer)
	{
		mObjectSet.As<ResourceSet>()->SetBuffer(3, boneBuffer);
	}

	void ShadowRenderer::EndRecording(FPass &pass, const RenderBatch &batch)
	{
		const uint32_t objectCount = batch.InstanceCount();
		const uint32_t groups = (objectCount + 255) / 256;

		pass.BeginPhase("Upload Shadow Batch", EPhaseType::Transfer);
		pass.Emplace<CmdClearBuffer>()(mObjectBuffer);
		pass.Emplace<CmdClearBuffer>()(mIndirectBuffer);
		pass.Emplace<CmdClearBuffer>()(mVisibilityBuffer);
		pass.Emplace<CmdSetBufferData>()(mObjectBuffer, &objectCount, sizeof(uint32_t));
		pass.Emplace<CmdSetBufferData>()(mObjectBuffer, batch.ObjectDatas.data(), sizeof(ObjectData) * objectCount, 16U);
		pass.Emplace<CmdSetBufferData>()(mIndirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
		pass.EndPhase();

		pass.BeginPhase("Update Shadow Data", EPhaseType::Transfer);
		pass.Emplace<CmdSetBufferData>()(mShadowBuffer, &mShadowData, sizeof(ShadowData));
		pass.EndPhase();

		const auto &num_shadow_maps = glm::compAdd(mShadowData.NumShadowMaps);
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

		auto buffer = mShadowBuffer;

		if (auto count = mShadowData.NumShadowMaps.x; count > 0)
		{
			auto fbo = mShadowFramebuffers[0];
			auto material = mShadowMaterials[0];

			mCullingMaterial.As<Material>()->SetParam("LightType", 0);

			for (uint32_t d = 0; d < count; ++d)
			{
				for (uint32_t c = 0; c < 4; ++c)
				{
					pass.BeginPhase("Reset Directional Shadow Culling", EPhaseType::Transfer);
					pass.Emplace<CmdSetBufferData>()(mIndirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
					pass.Emplace<CmdClearBuffer>()(mVisibilityBuffer);
					pass.EndPhase();

					mCullingMaterial.As<Material>()->SetParam("LightIndex", MaterialParam{d});
					mCullingMaterial.As<Material>()->SetParam("LightFace", MaterialParam{c});
					pass.BeginPhase("Cull Directional Shadows", EPhaseType::Compute);
					pass.UseBuffer(mIndirectBuffer, EBufferUsage::StorageWrite);
					pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageWrite);
					pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
					pass.BindResourceSet(mGlobalSet);
					pass.BindResourceSet(mObjectSet);
					pass.Emplace<CmdBindMaterial>()(mCullingMaterial.As<Material>());
					pass.Emplace<CmdDispatch>()(groups, 1, 1);
					pass.EndPhase();

					material.As<Material>()->SetParam("LightIndex", MaterialParam{d});
					material.As<Material>()->SetParam("CascadeIndex", MaterialParam{c});
					pass.BeginPhase("Generate Directional Shadows", EPhaseType::Graphics);
					pass.UseFramebuffer(fbo, ImageSubresourceRange{.BaseArrayLayer = d * 4 + c});
					pass.UseBuffer(buffer, EBufferUsage::StorageRead);
					pass.UseBuffer(mIndirectBuffer, EBufferUsage::IndirectRead);
					pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageRead);
					pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
					pass.BindResourceSet(mGlobalSet);
					pass.BindResourceSet(mObjectSet);
					pass.Emplace<CmdBindPipeline>()(mDirectionalPipeline);
					draw_meshes(pass, mIndirectBuffer, batch, material);

					pass.EndPhase();
				}
			}
		}

		if (auto count = mShadowData.NumShadowMaps.y; count > 0)
		{
			auto fbo = mShadowFramebuffers[1];
			auto material = mShadowMaterials[1];

			mCullingMaterial.As<Material>()->SetParam("LightType", 1);

			for (uint32_t p = 0; p < count; ++p)
			{
				mCullingMaterial.As<Material>()->SetParam("LightIndex", p);

				for (uint32_t face = 0; face < 6; ++face)
				{
					mCullingMaterial.As<Material>()->SetParam("LightFace", face);
					pass.BeginPhase("Reset Point Shadow Culling", EPhaseType::Transfer);
					pass.Emplace<CmdSetBufferData>()(mIndirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
					pass.Emplace<CmdClearBuffer>()(mVisibilityBuffer);
					pass.EndPhase();

					pass.BeginPhase("Cull Point Shadows", EPhaseType::Compute);
					pass.UseBuffer(mIndirectBuffer, EBufferUsage::StorageWrite);
					pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageWrite);
					pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
					pass.BindResourceSet(mGlobalSet);
					pass.BindResourceSet(mObjectSet);
					pass.Emplace<CmdBindMaterial>()(mCullingMaterial.As<Material>());
					pass.Emplace<CmdDispatch>()(groups, 1, 1);
					pass.EndPhase();

					material.As<Material>()->SetParam("LightIndex", MaterialParam{p});
					material.As<Material>()->SetParam("LightFace", MaterialParam{face});
					pass.BeginPhase("Generate PointLight ShadowMaps", EPhaseType::Graphics);
					pass.UseFramebuffer(fbo, ImageSubresourceRange{.BaseArrayLayer = CubeFaceLayer(p, face)});
					pass.UseBuffer(buffer, EBufferUsage::StorageRead);
					pass.UseBuffer(mIndirectBuffer, EBufferUsage::IndirectRead);
					pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageRead);
					pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
					pass.BindResourceSet(mGlobalSet);
					pass.BindResourceSet(mObjectSet);
					pass.Emplace<CmdBindPipeline>()(mPipeline);
					draw_meshes(pass, mIndirectBuffer, batch, material);

					pass.EndPhase();
				}
			}
		}

		if (auto count = mShadowData.NumShadowMaps.z; count > 0)
		{
			auto fbo = mShadowFramebuffers[2];
			auto material = mShadowMaterials[2];

			mCullingMaterial.As<Material>()->SetParam("LightType", 2);

			for (uint32_t s = 0; s < count; ++s)
			{
				pass.BeginPhase("Reset Spot Shadow Culling", EPhaseType::Transfer);
				pass.Emplace<CmdSetBufferData>()(mIndirectBuffer, batch.DrawCommands.data(), sizeof(MultiDrawIndirectCommand) * batch.DrawCommands.size());
				pass.Emplace<CmdClearBuffer>()(mVisibilityBuffer);
				pass.EndPhase();

				mCullingMaterial.As<Material>()->SetParam("LightIndex", MaterialParam{s});
				pass.BeginPhase("Cull Spot Shadows", EPhaseType::Compute);
				pass.UseBuffer(mIndirectBuffer, EBufferUsage::StorageWrite);
				pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageWrite);
				pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
				pass.BindResourceSet(mGlobalSet);
				pass.BindResourceSet(mObjectSet);
				pass.Emplace<CmdBindMaterial>()(mCullingMaterial.As<Material>());
				pass.Emplace<CmdDispatch>()(groups, 1, 1);
				pass.EndPhase();

				material.As<Material>()->SetParam("LightIndex", MaterialParam{s});
				pass.BeginPhase("Generate SpotLight Shadows", EPhaseType::Graphics);
				pass.UseFramebuffer(fbo, ImageSubresourceRange{.BaseArrayLayer = s});
				pass.UseBuffer(buffer, EBufferUsage::StorageRead);
				pass.UseBuffer(mIndirectBuffer, EBufferUsage::IndirectRead);
				pass.UseBuffer(mVisibilityBuffer, EBufferUsage::StorageRead);
				pass.UseBuffer(mObjectBuffer, EBufferUsage::StorageRead);
				pass.BindResourceSet(mGlobalSet);
				pass.BindResourceSet(mObjectSet);
				pass.Emplace<CmdBindPipeline>()(mPipeline);
				draw_meshes(pass, mIndirectBuffer, batch, material);

				pass.EndPhase();
			}
		}
	}

	void ShadowRenderer::SubmitDirectionalLight(const FShadowCascadedCreateInfo &info)
	{
		auto &shadow_data = mShadowData;
		auto &k = shadow_data.NumShadowMaps.x;
		if (k >= sMaxLights)
			return;

		const auto &near = info.CameraNearFar.x;
		const auto &far = info.CameraNearFar.y;

		float splits[4]{0, 0, 0, 0};
		float previousSplit = near;

		for (uint32_t i = 0; i < 4; i++)
		{
			float p = float(i + 1) / 4.0f;
			float logSplit = near * glm::pow(far / near, p);
			float uniformSplit = near + (far - near) * p;
			splits[i] = glm::mix(uniformSplit, logSplit, 0.95f);
		}

		float cascadeExtents[4] = {20.0f, 45.0f, 90.0f, 180.f};

		glm::vec3 center = {0, 0, 0};

		for (uint32_t cascade = 0; cascade < 4; cascade++)
		{
			float cascadeStart = previousSplit;
			float cascadeEnd = splits[cascade];
			previousSplit = cascadeEnd;

			shadow_data.DirProjections[k].Cascades[cascade].SplitData.x = cascadeEnd;

			const float extent = cascadeExtents[cascade];
			const float radius = extent * 0.5f;

			const auto lightPos = -info.LightDirection * extent;
			const auto up = glm::abs(lightPos.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
			glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0, 1, 0));

			auto proj = glm::ortho(-extent, extent, -extent, extent, -extent * 8.0f, extent * 8.0f);
			shadow_data.DirProjections[k].Cascades[cascade].ViewProjection = proj * lightView;
			shadow_data.DirProjections[k].Cascades[cascade].Frustum = Frustum(proj * lightView);
		}

		k++;
	}

	void ShadowRenderer::SubmitSpotLight(const FShadowFrustumCreateInfo &info)
	{
		auto &shadow_data = mShadowData;
		auto &i = shadow_data.NumShadowMaps.z;
		if (i >= sMaxLights)
			return;

		auto up = glm::abs(info.LightDirection.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
		auto proj = glm::perspective<float>(glm::radians(info.LightAngleNearFar.x), 1.f, info.LightAngleNearFar.y, info.LightAngleNearFar.z);
		auto view = glm::lookAt(info.LightPosition, info.LightPosition + info.LightDirection, up);

		shadow_data.SpotProjections[i].ViewProjection = proj * view;
		shadow_data.SpotProjections[i].Frustum = Frustum(proj * view);
		i++;
	}

	void ShadowRenderer::SubmitPointLight(const FShadowCubeCreateInfo &info)
	{
		auto &shadow_data = mShadowData;
		auto &i = shadow_data.NumShadowMaps.y;
		if (i >= sMaxLights)
			return;

		auto proj = glm::perspective(glm::radians(90.0f), 1.f, info.LightNearFar.x, info.LightNearFar.y);

		for (int j = 0; j < 6; j++)
		{
			auto view = glm::lookAt(info.LightPosition, info.LightPosition + point_directions[j].normal, point_directions[j].up);
			shadow_data.PointShadowInfos[i].ShadowViewProjections[j] = proj * view;
			shadow_data.PointShadowInfos[i].Frustums[j] = Frustum(proj * view);
		}

		shadow_data.PointShadowInfos[i].ShadowNearFar = glm::vec4(info.LightNearFar, 0.0, 0.0);
		i++;
	}

	BufferPtr ShadowRenderer::GetBuffer()
	{
		return mShadowBuffer;
	}

	TexturePtr ShadowRenderer::GetDirShadowMap()
	{
		return mShadowFramebuffers[0].As<Framebuffer>()->GetDepthAttachment();
	}

	TexturePtr ShadowRenderer::GetPointShadowMap()
	{
		return mShadowFramebuffers[1].As<Framebuffer>()->GetDepthAttachment();
	}

	TexturePtr ShadowRenderer::GetSpotShadowMap()
	{
		return mShadowFramebuffers[2].As<Framebuffer>()->GetDepthAttachment();
	}

	void ShadowRenderer::InitializeBuffers()
	{
		mObjectBuffer = BufferFactory::Create(OBJECT_BUFFER_SIZE, EBufferType::StorageBuffer);
		mIndirectBuffer = BufferFactory::Create(DRAWCOMMAND_BUFFER_SIZE, EBufferType::IndirectBuffer | EBufferType::StorageBuffer);
		mVisibilityBuffer = BufferFactory::Create(VISIBILITY_BUFFER_SIZE, EBufferType::StorageBuffer);
	}

	void ShadowRenderer::InitializeSets()
	{
		mGlobalSet = ResourceSetFactory::Create(RendererTemplates::Global());
		mObjectSet = ResourceSetFactory::Create(RendererTemplates::Object());

		mGlobalSet.As<ResourceSet>()->SetBuffer(5, mShadowBuffer);

		mObjectSet.As<ResourceSet>()->SetBuffer(0, mObjectBuffer);
		mObjectSet.As<ResourceSet>()->SetBuffer(1, mIndirectBuffer);
		mObjectSet.As<ResourceSet>()->SetBuffer(2, mVisibilityBuffer);
	}

} // namespace BHive