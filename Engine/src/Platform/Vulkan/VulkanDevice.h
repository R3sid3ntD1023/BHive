//#pragma once
//
//#include "Core/Core.h"
//#include "VulkanCore.h"
//
//namespace BHive
//{
//	
//	class VulkanDevice
//	{
//	public:
//		VulkanDevice() = default;
//
//		~VulkanDevice();
//
//		void Init(vk::raii::PhysicalDevice &physicalDevice, vk::raii::SurfaceKHR& surface);
//
//		void WaitIdle();
//
//		vk::raii::Queue &GetGraphicsQueue() { return mQueueFamilies.GraphicsQueue; }
//
//		uint32_t GetGraphicsQueueIndex() const { return mQueueFamilies.GraphicsQueueIndex; }
//
//		operator vk::raii::Device &() { return mDevice; }
//
//		vk::raii::Device& operator*() { return mDevice; }
//
//		static std::vector<const char *> GetRequiredExtensions();
//
//	private:
//		vk::raii::Device mDevice = VK_NULL_HANDLE;
//
//		FQueueFamilies mQueueFamilies;
//
//	};
//}