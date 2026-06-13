#pragma once

#include "gfx/RenderGraph.h"
#include "Platform/Vulkan/VulkanCore.h"
#include "Platform/Vulkan/material/VulkanBackendMaterial.h"

namespace BHive
{
	class VulkanPipeline;

	class FVulkanComputeBindings : public FComputeBindings
	{
	public:
		FVulkanComputeBindings(VulkanPipeline* pipeline);

		virtual void StorageImage(const char *name, const FImageInfo &info) override;

		virtual void SampledImage(const char *name, const FImageInfo &info) override;

		virtual void Set(const char *name, const void *data, size_t size) override;

		void Bind(vk::CommandBuffer cmd) const;

		const auto &GetBoundImages() const { return mImages; }

	private:
		Scope<VulkanBackendMaterial> mBackendMaterial;
		VulkanPipeline* mPipeline = nullptr;
		std::vector<std::pair<FImageInfo, bool>> mImages;
	};
}