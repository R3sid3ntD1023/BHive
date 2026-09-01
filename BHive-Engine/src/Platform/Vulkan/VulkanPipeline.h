#pragma once

#include "VulkanCore.h"
#include "gfx/Pipeline.h"

namespace BHive
{

	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline() = default;

		virtual void Init(const PipelineState *state) override;

		virtual void Bind(vk::CommandBuffer cmd, uint32_t frame, uint32_t numAttachments);

	private:
		Scope<Pipeline::PipelineState> mState;
	};
} // namespace BHive