#include "CullingPipeline.h"
#include "gfx/Shader.h"
#include "indirect_mesh/IndirectMesh.h"
#include "mesh/IRenderableAsset.h"
#include "renderers/Renderer.h"
#include "volumes/SphereVolume.h"

namespace BHive
{
	CullingPipeline::CullingPipeline()
	{
		mCullingShader = ShaderLibrary::Load(RESOURCE_PATH "Shaders/CullingShader.glsl");
	}

	void CullingPipeline::SetTestFrustum(const Frustum &frustum)
	{
		mTestFrustum = frustum;
	}

	void CullingPipeline::OnPipelineEnd()
	{
		RenderPipeline::OnPipelineEnd();

		auto &objects = mPipelineData.Objects;

		mCullingShader->Bind();

		size_t i = 0;
		for (auto &object : objects)
		{
			auto indirect_mesh = object.Mesh;
			if (!indirect_mesh)
				continue;

			const auto &mesh = indirect_mesh->GetRenderable();
			const auto &bounds = mesh->GetBoundingBox();
			FSphereVolume volume = GenerateSphereFromAABB(bounds);
			bool is_in_frustum = volume.InFrustum(mTestFrustum, object.Transform);

			srand(i++);
			glm::vec3 color = {(float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX};
			mCullingShader->SetUniform("uColor", color);
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

			LineRenderer::DrawSphere(volume.Radius, 16, {}, 0xFF00FFFF, object.Transform);
		}

		mCullingShader->UnBind();

		Renderer::End();
	}
} // namespace BHive
