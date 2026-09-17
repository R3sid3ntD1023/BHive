#include "VulkanMemory.h"
#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace BHive
{
	void ImageStateTracker::Initialize(uint32_t layers, uint32_t mips, const ImageState &initial, uint32_t faces)
	{
		ASSERT(layers > 0 && mips > 0, "layers and levels must be 1 or greater!")

		for (uint32_t layer = 0; layer < layers; ++layer)
		{

			for (uint32_t face = 0; face < faces; ++face)
			{
				auto arrayLayer = layer * faces + face;

				for (uint32_t mip = 0; mip < mips; ++mip)
				{
					States[{arrayLayer, mip}] = initial;
				}
			}
		}
	}

	ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip)
	{
		return States[{layer, mip}];
	}

	const ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip) const
	{
		return States.at({layer, mip});
	}
} // namespace BHive
