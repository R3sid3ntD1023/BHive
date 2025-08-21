#include "SceneRenderer.h"
#include "renderers/Renderer.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "mesh/primitives/Quad.h"
#include "gfx/RenderCommand.h"
#include "core/math/Transform.h"
#include "renderers/postprocessing/Bloom.h"
#include "renderers/PMREMGenerator.h"
#include "importers/TextureImporter.h"
#include "postprocessing/Aces.h"
#include "gfx/textures/Texture2D.h"
#include "ShadowRenderer.h"

#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "gfx/StorageBuffer.h"
#include "gfx/UniformBuffer.h"

namespace BHive
{
#define SSBO_INDEX_PER_OBJECT_BINDING 1
#define SSBO_INSTANCE_BINDING 2
#define SSBO_BONE_BINDING 3
#define MAX_BONES 200

	struct FMeshRenderData
	{
		std::unordered_map<Ref<Material>, std::vector<FRenderData>> RenderData;
		Ref<StorageBuffer> BoneBuffer;
		Ref<StorageBuffer> WorldMatrixBuffer;
		Ref<StorageBuffer> InstanceBuffer;
	};

	void SceneRenderer::Initialize(uint32_t width, uint32_t height)
	{
		mMeshRenderData = CreateRef<FMeshRenderData>();
		mMeshRenderData->BoneBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_BONES);
		mMeshRenderData->WorldMatrixBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4));
		mMeshRenderData->InstanceBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * 1000);

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Width = width;
		specs.Height = height;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = CreateRef<Framebuffer>(specs);

		// Initialize bloom post-processing effect if enabled
		AddPostProcessingEffect(CreateRef<Bloom>(5, width, height, FBloomSettings{}));
		AddPostProcessingEffect(CreateRef<Aces>(width, height));

		// Create a final framebuffer for post-processing effects
		specs.Attachments.reset();
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFinalFramebuffer = CreateRef<Framebuffer>(specs);

		// Create a quad for rendering the final output
		mQuad = CreateRef<PQuad>();
		mQuadShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/ScreenQuad.glsl");

		// Initialize the PMREM generator
		EnvironmentMapGenerator.Initialize();

		// Load the environment map if it is not already set
		if (!sEnvironmentMap)
		{
			sEnvironmentMap = TextureLoader::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
			EnvironmentMapGenerator.SetEnvironmentMap(sEnvironmentMap);
		}

		mRenderSize = {width, height};
	}

	void SceneRenderer::Begin(const Camera *camera, const FTransform &view)
	{

		Renderer::Begin();
		ShadowRenderer::Begin();
		Renderer::SubmitCamera(camera->GetProjection(), view.Inverse());

		mMeshRenderData->RenderData.clear();
	}

	void SceneRenderer::End()
	{
		ShadowRenderer::End();

		auto num_lights = Renderer::GetNumLights();
		if (glm::compAdd(num_lights) > 0)
		{

			auto draw_meshes = [=]()
			{
				for (auto &[mat, objects] : mMeshRenderData->RenderData)
				{
					DrawMeshes(objects);
				};
			};

			RenderCommand::CullFront();
			if (num_lights.y > 0)
			{

				ShadowRenderer::BeginPointShadowPass();

				draw_meshes();

				ShadowRenderer::EndPointShadowPass();
			}

			if (num_lights.z > 0)
			{
				ShadowRenderer::BeginSpotShadowPass();

				draw_meshes();

				ShadowRenderer::EndSpotShadowPass();
			}

			RenderCommand::CullBack();
		}

		mFramebuffer->Bind();

		RenderCommand::Clear();

		EnvironmentMapGenerator.GetPreFilteredEnvironmentTetxure()->Bind(6);
		EnvironmentMapGenerator.GetIrradianceTexture()->Bind(7);
		EnvironmentMapGenerator.GetBDRFLUT()->Bind(8);

		static uint32_t shadow_map_bindings[] = {9, 10, 11};
		ShadowRenderer::BindShadowMaps(shadow_map_bindings);

		DrawMeshes();

		while (mCommands.size())
		{
			mCommands.top()();
			mCommands.pop();
		}

		Renderer::End();

		mFramebuffer->UnBind();

		auto texture = mFramebuffer->GetColorAttachment(0);

		for (const auto &effect : mPostProcessingEffects)
		{
			texture = effect->Process(texture);
		}

		mFinalFramebuffer->Bind();

		RenderCommand::Clear();

		mQuadShader->Bind();

		texture->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());

		mFinalFramebuffer->UnBind();
	}

	void SceneRenderer::SetEnvironmentMap(const Ref<Texture2D> &environment)
	{
		sEnvironmentMap = environment;
		EnvironmentMapGenerator.SetEnvironmentMap(environment);
	}

	void SceneRenderer::AddPostProcessingEffect(const Ref<PostProcessor> &processor)
	{
		mPostProcessingEffects.push_back(processor);
	}

	void SceneRenderer::SubmitMesh(const Ref<StaticMesh> &mesh, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		SubmitMesh(mesh, mesh->GetMaterialTable(), transform, instances, instanceCount);
	}

	void SceneRenderer::SubmitMesh(const Ref<SkeletalMesh> &mesh, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		SubmitMesh(mesh, mesh->GetMaterialTable(), pose, transform, instances, instanceCount);
	}

	void SceneRenderer::SubmitMesh(const Ref<StaticMesh> &mesh, const MaterialTable &materials, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();

		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto data = FRenderData{
				.VertexArray = mesh->GetVertexArray(), .SubMesh = sub_mesh, .Transform = transform, .Bounds = mesh->GetBoundingBox(), .Instances = instances, .InstanceCount = instanceCount};

			mMeshRenderData->RenderData[material].emplace_back(data);
		}
	}

	void
	SceneRenderer::SubmitMesh(const Ref<SkeletalMesh> &mesh, const MaterialTable &materials, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();
		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto data = FRenderData{
				.VertexArray = mesh->GetVertexArray(),
				.SubMesh = sub_mesh,
				.Transform = transform,
				.Bounds = mesh->GetBoundingBox(),
				.Pose = &pose,
				.Instances = instances,
				.InstanceCount = instanceCount};

			mMeshRenderData->RenderData[material].emplace_back(data);
		}
	}

	void SceneRenderer::SubmitCommand(const std::function<void()> cmd)
	{
		mCommands.push(cmd);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		mFramebuffer->Resize(width, height);

		mFinalFramebuffer->Resize(width, height);

		for (auto &effect : mPostProcessingEffects)
		{
			effect->Resize(width, height);
		}

		mRenderSize = {width, height};
	}

	const Ref<Texture> &SceneRenderer::GetColorAttachment(uint32_t index) const
	{
		return mFinalFramebuffer->GetColorAttachment(index);
	}

	const Ref<Texture> &SceneRenderer::GetDepthAttachment() const
	{
		return mFinalFramebuffer->GetDepthAttachment();
	}

	const Ref<Texture2D> &SceneRenderer::GetEnvironmentMap() const
	{
		return sEnvironmentMap;
	}

	glm::uvec2 SceneRenderer::GetSize() const
	{
		return mRenderSize;
	}

	void SceneRenderer::RenderToScreen()
	{
		RenderCommand::Clear();

		mQuadShader->Bind();

		mFinalFramebuffer->GetColorAttachment()->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());
	}

	void SceneRenderer::DrawMeshes()
	{
		for (auto &[mat, objects] : mMeshRenderData->RenderData)
		{
			auto shader = mat->GetShader();
			shader->Bind();
			mat->Submit(shader);

			DrawMeshes(objects);

			shader->UnBind();
		}
	}

	void SceneRenderer::DrawMeshes(const std::vector<FRenderData> &datas)
	{
		for (const auto &data : datas)
		{
			auto matrix = data.Transform * data.SubMesh.Transformation;
			mMeshRenderData->WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
			mMeshRenderData->WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

			if (data.InstanceCount > 0)
			{
				mMeshRenderData->InstanceBuffer->SetData(data.Instances, sizeof(glm::mat4) * data.InstanceCount);
				mMeshRenderData->InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
			}

			if (data.Pose)
			{
				const auto &joints = data.Pose->GetTransformsJointSpace();
				mMeshRenderData->BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
				mMeshRenderData->BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
			}

			RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *data.VertexArray, data.SubMesh.StartVertex, data.SubMesh.StartIndex, data.SubMesh.IndexCount, data.InstanceCount);
		}
	}

	bool SceneRenderer::IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform)
	{
		if (!mesh)
			return true;

		const auto &bounds = mesh->GetBoundingBox();
		const auto &frustum = Renderer::GetFrustum();

		auto volume = FSphereVolume(bounds.GetCenter(), bounds.GetRadius());
		return !volume.InFrustum(frustum, FTransform(transform));
	}

	void SceneRenderer::SortObjects()
	{
		static auto sorter = [=](const FRenderData &a, const FRenderData &b)
		{
			const auto &A = a.Transform[3];
			const auto &B = b.Transform[3];
			const auto &C = CameraBuffer::Get().GetCameraData().CameraPosition;

			const auto distA = glm::distance(A, C);
			const auto distB = glm::distance(B, C);

			return distA < distB;
		};

		for (auto &[mat, objdatas] : mMeshRenderData->RenderData)
		{
			auto &objects = objdatas;
			std::sort(objects.begin(), objects.end(), sorter);
		}
	}

	REFLECT(FRenderSettings)
	{
		BEGIN_REFLECT(FRenderSettings);
	}

	REFLECT(SceneRenderer)
	{
		BEGIN_REFLECT(SceneRenderer)
		REFLECT_PROPERTY("PostProcessEffects", mPostProcessingEffects)
		REFLECT_PROPERTY("EnvironmentMap", GetEnvironmentMap, SetEnvironmentMap);
	}
} // namespace BHive