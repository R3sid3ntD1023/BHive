#include "ShadowRenderer.h"
#include "RenderBatch.h"
#include "Renderer.h"
#include "SceneRenderer.h"
#include "core/math/Frustum.h"
#include "gfx/ShaderManager.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/shader/Shader.h"

#define DIRECTIONAL_SHADOWMAP_SIZE 1024
#define POINT_SHADOWMAP_SIZE 256
#define SPOT_SHADOWMAP_SIZE 256

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

	struct alignas(16) CascadeShadow
	{
		glm::mat4 ViewProjection{1.0f};
		glm::vec4 SplitData{0.0f}; // x= far plane
		Frustum Frustum{};
	};

	struct alignas(16) FDirectionalShadowData
	{
		CascadeShadow Cascades[4];
	};

	struct alignas(16) FPointLightDataData
	{
		glm::mat4 ShadowViewProjections[6];
		glm::vec4 ShadowNearFar;
		Frustum Frustums[6];
	};

	struct alignas(16) FSpotLightData
	{
		glm::mat4 ViewProjection{1.0f};
		Frustum Frustum{};
	};

	struct alignas(16) FShadowData
	{
		glm::uvec4 NumShadowMaps = {0, 0, 0, 0}; // {Dir, Point, Spot}
		std::array<FDirectionalShadowData, ShadowRenderer::sMaxLights> DirProjections = {};
		std::array<FPointLightDataData, ShadowRenderer::sMaxLights> PointShadowInfos = {};
		std::array<FSpotLightData, ShadowRenderer::sMaxLights> SpotProjections = {};
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

	void ShadowRenderer::Init()
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
		shadowTex.CreateInfo.ArrayLayers = sMaxLights * 4;

		dir_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);

		shadowTex.CreateInfo.ArrayLayers = sMaxLights;
		spot_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);

		shadowTex.Type = ETextureType::TEXTURE_CUBE_MAP_ARRAY;
		point_shadow_fbo_spec.Attachments.SetDepthAttachment(shadowTex);

		auto &shadow_passes = mShadowRenderData->ShadowPasses;
		shadow_passes.FBOs[0] = FramebufferFactory::Create(dir_shadow_fbo_spec);
		shadow_passes.FBOs[1] = FramebufferFactory::Create(point_shadow_fbo_spec);
		shadow_passes.FBOs[2] = FramebufferFactory::Create(spot_shadow_fbo_spec);

		shadow_passes.ShadowMats[0] = MaterialFactory::Create("ShadowDirectionalLight.glsl");
		shadow_passes.ShadowMats[1] = MaterialFactory::Create("ShadowPointLight.glsl");
		shadow_passes.ShadowMats[2] = MaterialFactory::Create("ShadowSpotLight.glsl");
		mCullingMaterial = MaterialFactory::Create("ShadowCulling.glsl");

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

		InitializeBuffers();
		InitializeSets();
	}

	void ShadowRenderer::BeginRecording()
	{
		mShadowRenderData->ShadowData.NumShadowMaps = {0, 0, 0, 0};
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

		auto buffer = mShadowRenderData->ShadowBuffer;

		if (auto count = mShadowRenderData->ShadowData.NumShadowMaps.x; count > 0)
		{
			auto fbo = mShadowRenderData->ShadowPasses.FBOs[0];
			auto material = mShadowRenderData->ShadowPasses.ShadowMats[0];

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
					pass.Emplace<CmdBindPipeline>()(mPipeline);
					draw_meshes(pass, mIndirectBuffer, batch, material);

					pass.EndPhase();
				}
			}
		}

		if (auto count = mShadowRenderData->ShadowData.NumShadowMaps.y; count > 0)
		{
			auto fbo = mShadowRenderData->ShadowPasses.FBOs[1];
			auto material = mShadowRenderData->ShadowPasses.ShadowMats[1];

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

		if (auto count = mShadowRenderData->ShadowData.NumShadowMaps.z; count > 0)
		{
			auto fbo = mShadowRenderData->ShadowPasses.FBOs[2];
			auto material = mShadowRenderData->ShadowPasses.ShadowMats[2];

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
		auto &shadow_data = mShadowRenderData->ShadowData;
		auto &k = shadow_data.NumShadowMaps.x;
		if (k >= sMaxLights)
			return;

		const auto &frustum = info.CameraFrustum;
		const auto &pts = frustum.GetPoints();
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

		for (uint32_t cascade = 0; cascade < 4; cascade++)
		{
			float cascadeStart = previousSplit;
			float cascadeEnd = splits[cascade];
			previousSplit = cascadeEnd;

			float splitNear = (cascadeStart - near) / (far - near);
			float splitFar = (cascadeEnd - near) / (far - near);

			glm::vec3 cascadePoints[8];
			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 nearCorner = pts[i];
				glm::vec3 farCorner = pts[i + 4];

				cascadePoints[i] = glm::mix(nearCorner, farCorner, splitNear);
				cascadePoints[i + 4] = glm::mix(nearCorner, farCorner, splitFar);
			}

			shadow_data.DirProjections[k].Cascades[cascade].SplitData.x = cascadeEnd;

			glm::vec3 center(0.0f);

			for (auto &p : cascadePoints)
			{
				center += p;
			}
			center /= 8.0f;

			float radius = 0.0f;
			for (auto &p : cascadePoints)
			{
				radius = glm::max(radius, glm::distance(center, p));
			}
			radius = glm::ceil(radius * 16.0f) / 16.0f;

			const auto up = glm::abs(info.LightDirection.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
			glm::mat4 tempView = glm::lookAt(center - info.LightDirection * radius * 2.0f, center, up);
			glm::vec4 centerLS = tempView * glm::vec4(center, 1.0f);

			float extent = radius * 2.0f;
			float texelSize = extent / DIRECTIONAL_SHADOWMAP_SIZE;

			// snap center
			centerLS.x = glm::floor(centerLS.x / texelSize) * texelSize;
			centerLS.y = glm::floor(centerLS.y / texelSize) * texelSize;
			center = glm::inverse(tempView) * centerLS;

			const auto lightView = glm::lookAt(center - info.LightDirection * radius * 2.0f, center, up);

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::lowest();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::lowest();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::lowest();

			for (auto &p : cascadePoints)
			{
				glm::vec4 lp = lightView * glm::vec4(p, 1.0f);

				minX = std::min(minX, lp.x);
				maxX = std::max(maxX, lp.x);
				minY = std::min(minY, lp.y);
				maxY = std::max(maxY, lp.y);
				minZ = std::min(minZ, lp.z);
				maxZ = std::max(maxZ, lp.z);
			}

			auto proj = glm::ortho(minX, maxX, minY, maxY, minZ - radius, maxZ + radius);
			shadow_data.DirProjections[k].Cascades[cascade].ViewProjection = proj * lightView;
			shadow_data.DirProjections[k].Cascades[cascade].Frustum = Frustum(proj * lightView);
		}

		k++;
	}

	void ShadowRenderer::SubmitSpotLight(const FShadowFrustumCreateInfo &info)
	{
		auto &shadow_data = mShadowRenderData->ShadowData;
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
		auto &shadow_data = mShadowRenderData->ShadowData;
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
		return mShadowRenderData->ShadowBuffer;
	}

	TexturePtr ShadowRenderer::GetDirShadowMap()
	{
		return mShadowRenderData->ShadowPasses.FBOs[0].As<Framebuffer>()->GetDepthAttachment();
	}

	TexturePtr ShadowRenderer::GetPointShadowMap()
	{
		return mShadowRenderData->ShadowPasses.FBOs[1].As<Framebuffer>()->GetDepthAttachment();
	}

	TexturePtr ShadowRenderer::GetSpotShadowMap()
	{
		return mShadowRenderData->ShadowPasses.FBOs[2].As<Framebuffer>()->GetDepthAttachment();
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

		mGlobalSet.As<ResourceSet>()->SetBuffer(5, mShadowRenderData->ShadowBuffer);

		mObjectSet.As<ResourceSet>()->SetBuffer(0, mObjectBuffer);
		mObjectSet.As<ResourceSet>()->SetBuffer(1, mIndirectBuffer);
		mObjectSet.As<ResourceSet>()->SetBuffer(2, mVisibilityBuffer);
	}

} // namespace BHive