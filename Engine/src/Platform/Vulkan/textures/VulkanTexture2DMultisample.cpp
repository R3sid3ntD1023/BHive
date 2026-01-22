#include "VulkanTexture2DMultisample.h"

namespace BHive
{
	VulkanTexture2DMultisample::VulkanTexture2DMultisample(uint32_t width, uint32_t height, uint32_t samples, const FTextureCreateInfo &create_info)
		: mWidth(width),
		  mHeight(height),
		  mSamples(samples),
		  mCreateInfo(create_info),
		  mInfo(create_info)
	{
	}

} // namespace BHive