#pragma once

#include "gfx/RenderGraph.h"
#include "VulkanCore.h"
#include "VulkanBackendMaterial.h"

namespace BHive
{
	class VulkanPipeline;

	class FVulkanComputeBindings : public FComputeBindings
	{
	public:
		FVulkanComputeBindings(VulkanPipeline* pipeline);

		virtual void Bind(const char *name, Ref<Texture> tex, ImageSubresourceRange range = {}) override;

		virtual void Set(const char *name, const void *data, size_t size) override;

		void Bind(vk::CommandBuffer cmd) const;

	private:
		Scope<VulkanBackendMaterial> mBackendMaterial;
		VulkanPipeline* mPipeline = nullptr;
	};
}