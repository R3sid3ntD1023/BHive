#pragma once

#include "gfx/Pipeline.h"

namespace BHive
{
	class GLPipeline : public Pipeline
	{
	public:
		virtual void Init(const PipelineState *state) override;

		virtual void Bind(uint32_t numAttachments);

	private:
		Scope<Pipeline::PipelineState> mState;
	};
} // namespace BHive