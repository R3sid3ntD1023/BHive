#pragma once

#include "IResourceFactory.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	struct BHIVE_API PipelineFactory : public IResourceFactory<Pipeline>
	{
		static PipelinePtr Create();

		static PipelinePtr Create(const Pipeline::PipelineState &state);
	};
} // namespace BHive