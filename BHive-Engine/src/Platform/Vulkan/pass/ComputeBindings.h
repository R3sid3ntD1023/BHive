#pragma once

#include "gfx/RenderGraph.h"
#include "Platform/Vulkan/VulkanCore.h"

namespace BHive
{
	class VulkanPipeline;
	class VulkanBackendMaterial;

	class FVulkanComputeBindings : public FComputeBindings
	{
	public:
		FVulkanComputeBindings(const Ref<VulkanPipeline>& pipeline);

		virtual void StorageImage(const char *name, const FImageInfo &info) override;

		virtual void SampledImage(const char *name, const FImageInfo &info) override;

		virtual void Set(const char *name, const void *data, size_t size) override;

		virtual void Bind() const override;

		void BindImmediate(vk::CommandBuffer cmd) const;

		const auto &GetBoundImages() const { return mImages; }

	private:
		Ref<VulkanBackendMaterial> mBackendMaterial;
		Ref<VulkanPipeline> mPipeline;
		std::vector<std::pair<FImageInfo, bool>> mImages;
	};
}