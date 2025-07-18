#include "CullingPipeline.h"
#include "gfx/Shader.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "mesh/BaseMesh.h"
#include "renderers/Renderer.h"
#include "math/volumes/SphereVolume.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	CullingPipeline::CullingPipeline()
	{
		mCullingShader = ShaderManager::Get().Get("CullingShader.glsl");
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
	}
} // namespace BHive
