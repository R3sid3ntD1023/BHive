#pragma once

#include "RenderPipeline.h"

namespace BHive
{

	class UniverseRenderPipeline : public RenderPipeline
	{

	protected:
		void OnPipelineEnd() override;
	};
} // namespace BHive