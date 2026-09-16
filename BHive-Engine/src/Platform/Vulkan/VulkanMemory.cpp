#include "VulkanMemory.h"
#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace BHive
{
	void ImageStateTracker::Initialize(uint32_t layers, uint32_t mips, const ImageState &initial)
	{
		ASSERT(layers > 0 && mips > 0, "layers and levels must be 1 or greater!")

		for (uint32_t layer = 0; layer < layers; ++layer)
		{
			for (uint32_t mip = 0; mip < mips; ++mip)
			{
				States[{layer, 0, mip}] = initial;
			}
		}
	}

	ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip)
	{
		return States[{layer, 0, mip}];
	}

	const ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip) const
	{
		return States.at({layer, 0, mip});
	}
} // namespace BHive
