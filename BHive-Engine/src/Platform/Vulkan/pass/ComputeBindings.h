#pragma once

#include "gfx/RenderGraph.h"

namespace BHive
{
	class VulkanPipeline;
	class IMaterialBackendInterface;

	class FVulkanComputeBindings : public FComputeBindings
	{
	public:
		FVulkanComputeBindings(const Ref<VulkanPipeline>& pipeline);

		virtual void StorageImage(const char *name, const Ref<Texture> &tex, uint32_t mip = 0) override;

		virtual void SampledImage(const char *name, const Ref<Texture> &tex, uint32_t mip = 0) override;

		virtual void Set(const char *name, const void *data, size_t size) override;

		virtual void Bind() const override;

	private:
		Ref<IMaterialBackendInterface> mBackendMaterial;
		Ref<VulkanPipeline> mPipeline;
	};
}