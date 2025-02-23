#include "CullingPipeline.h"
#include "gfx/Shader.h"
#include "indirect_mesh/IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "volumes/SphereVolume.h"
#include "renderers/Renderer.h"

namespace BHive
{
	CullingPipeline::CullingPipeline()
	{
		mCullingShader = ShaderLibrary::Load(RESOURCE_PATH "Shaders/CullingShader.glsl");
	}

	void CullingPipeline::SubmitObject(const ObjectData &object)
	{
		mData.Objects.emplace_back(object);
	}

	void CullingPipeline::Begin(const glm::mat4 &proj, const glm::mat4 &view)
	{
		mData.Objects.clear();
		mData.CameraTransform = view;

		Renderer::Begin(proj, view);
	}

	void CullingPipeline::SetTestFrustum(const Frustum &frustum)
	{
		mData.TestFrustum = frustum;
	}

	void CullingPipeline::End()
	{
		static auto sorter = [this](const ObjectData &a, const ObjectData &b)
		{
			const auto &A = a.Transform.get_translation();
			const auto &B = b.Transform.get_translation();
			const auto &C = mData.CameraTransform.get_translation();

			const auto distA = glm::distance(A, C);
			const auto distB = glm::distance(B, C);

			return distA < distB;
		};

		auto &objects = mData.Objects;
		// sort objects based on distance from camera
		std::sort(objects.begin(), objects.end(), sorter);

		mCullingShader->Bind();
		for (auto &object : objects)
		{
			auto indirect_mesh = object.Renderable;
			if (!indirect_mesh)
				continue;

			const auto &mesh = indirect_mesh->GetRenderable();
			const auto &bounds = mesh->GetBoundingBox();
			FSphereVolume volume = GenerateSphereFromAABB(bounds);
			bool is_in_frustum = volume.InFrustum(mData.TestFrustum, object.Transform);

			mCullingShader->SetUniform("uIsInFrustum", is_in_frustum);
			mCullingShader->SetUniform("uInstanced", object.Instances > 0);

			if (!object.Instances)
			{
				indirect_mesh->Draw(object.Transform);
			}
			else
			{
				indirect_mesh->Draw(object.Transform, object.InstanceTransforms);
			}
		}
		mCullingShader->UnBind();

		Renderer::End();
	}

	CullingPipeline &CullingPipeline::GetPipeline()
	{
		static CullingPipeline pipeline;
		return pipeline;
	}
} // namespace BHive
