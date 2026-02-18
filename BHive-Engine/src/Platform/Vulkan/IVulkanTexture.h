#pragma once

#include "VulkanBackend.h"

namespace BHive
{
	class IVulkanTexture
	{
	public:

		virtual const vk::ImageView &GetImageView() const = 0;
	};
}