#pragma once

#include "core/Core.h"
#include "gfx/Pipeline.h"
#include  "VulkanBackend.h"

namespace BHive
{
	class BHIVE_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();

		~VulkanPipeline();

		virtual void Init(const PipelineState& state) override;

		virtual void Bind() override;

		virtual void UnBind() override;

		const vk::Pipeline &operator*() const { return mPipeline; }

	
	private:
		vk::raii::Device &mDevice;

		vk::raii::Pipeline mPipeline = VK_NULL_HANDLE;
	};
} // namespace BHive