#pragma once

#include "VulkanCore.h"

namespace BHive
{
	class VulkanInstance
	{
	public:
		VulkanInstance(const char *name, const char *engine_name = "NoEngine");
		~VulkanInstance();

		operator VkInstance() const { return mInstance; }

	private:
		bool CheckValidationLayerSupport();
		std::vector<const char *> GetRequiredExtensions();
		void CreateDebugMessenger();
		void PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	private:
		VkInstance mInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

		std::vector<const char *> mValidationLayers;
	};
} // namespace BHive