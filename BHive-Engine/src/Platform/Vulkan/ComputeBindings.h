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

		virtual void StorageImage(const char *name, const FImageInfo &info) override;

		virtual void SampledImage(const char *name, const FImageInfo &info) override;

		virtual void Set(const char *name, const void *data, size_t size) override;

		void Bind(vk::CommandBuffer cmd) const;

		const auto &GetSamplerImages() const { return mSamplers; }

		const auto &GetStorageImages() const { return mImages; }

		void TransitionStorageImagesAndClear(vk::raii::CommandBuffer &cmd, vk::ClearColorValue color);

		void TransitionStorageImagesToShader(vk::raii::CommandBuffer &cmd);

		void TransitionSamplerImagesToShader(vk::raii::CommandBuffer &cmd);

	private:
		Scope<VulkanBackendMaterial> mBackendMaterial;
		VulkanPipeline* mPipeline = nullptr;
		std::vector<FImageInfo> mSamplers;
		std::vector<FImageInfo> mImages;
	};
}