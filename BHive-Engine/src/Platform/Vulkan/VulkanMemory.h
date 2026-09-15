#pragma once

#include "ImageState.h"
#include "MemoryAllocator.h"
#include "gfx/NativeHandle.h"
#include "gfx/ResourceID.h"

namespace BHive
{
	struct ViewKey
	{
		uint32_t Layer = 0, Face = 0, Mip = 0;

		bool operator<(const ViewKey &rhs) const
		{
			if (Layer != rhs.Layer)
				return Layer < rhs.Layer;

			if (Face != rhs.Face)
				return Face < rhs.Face;

			return Mip < rhs.Mip;
		}
	};

	struct ImageViews
	{
		std::map<ViewKey, ResourceID> Views;
	};

	struct ImageStateTracker
	{
		std::vector<std::vector<ImageState>> MipStates;

		void Initialize(uint32_t layers, uint32_t mips, const ImageState &initial);

		ImageState &Get(uint32_t layer, uint32_t mip);

		const ImageState &Get(uint32_t layer, uint32_t mip) const;
	};

	struct GPUImage
	{
		~GPUImage();

		ResourceID Image{0};

		ImageViews Views;

		ResourceID Sampler{0};

		vk::ImageUsageFlags Usage;

		vk::ImageViewType ViewType;

		std::string DebugName;

		const vk::Image GetImage() const;

		const vk::Sampler GetSampler() const;

		vk::ImageView GetView(uint32_t layer, uint32_t face, uint32_t mip) const;

		bool IsValid() const { return Image; }

		operator bool() const { return IsValid(); }
	};

} // namespace BHive