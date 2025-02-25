#pragma once

#include "RenderPipeline.h"

namespace BHive
{

	class UniverseRenderPipeline : public RenderPipeline
	{
	public:
		virtual void Begin(const glm::mat4 &projection, const glm::mat4 &view);

	protected:
		void OnPipelineEnd() override;

	private:
		float mNear{}, mFar{};
	};
} // namespace BHive