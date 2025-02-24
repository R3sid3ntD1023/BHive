#pragma once

#include "core/Core.h"
#include "RenderPipeline.h"

namespace BHive
{
	class Shader;
	class UniformBuffer;

	class CullingPipeline : public RenderPipeline
	{

	public:
		CullingPipeline();

		void SetTestFrustum(const Frustum &frustum);

	protected:
		void OnPipelineEnd() override;

	private:
		Ref<Shader> mCullingShader;
		Frustum mTestFrustum;
		Ref<UniformBuffer> mCullingBuffer;
	};
} // namespace BHive