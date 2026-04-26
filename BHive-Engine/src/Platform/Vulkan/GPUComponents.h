#pragma once

#include "ImageComponent.h"
#include "ImageState.h"
#include "VulkanImageRegions.h"
#include "core/UUID.h"

namespace BHive
{
	struct DefaultViewComponent : public IImageComponent
	{
		UUID View;

		const vk::ImageView &Get() const;
	};

	struct MipViewComponent : IImageComponent
	{

		std::vector<std::vector<UUID>> Views; // [layer][mip]

		const vk::ImageView &Get(uint32_t layer, uint32_t mip) const;
	};

	struct FaceMipViewComponent : IImageComponent
	{
		std::vector<std::vector<std::vector<UUID>>> Views; // [layer][face][mip]

		const vk::ImageView &Get(uint32_t layer, uint32_t face, uint32_t mip) const;
	};

	struct CubeMipViewComponent : IImageComponent
	{
		std::vector<std::vector<UUID>> Views; // [layer][mip]

		const vk::ImageView &Get(uint32_t layer, uint32_t mip) const;
	};

	struct SamplerComponent : IImageComponent
	{
		UUID Sampler;

		const vk::Sampler &Get() const;
	};

	struct StateTrackingComponent : IImageComponent
	{
		std::vector<std::vector<ImageState>> MipStates;

		void Init(uint32_t layers, uint32_t mips, const ImageState &initial);

		ImageState &Get(uint32_t layer, uint32_t mip);
	};
}