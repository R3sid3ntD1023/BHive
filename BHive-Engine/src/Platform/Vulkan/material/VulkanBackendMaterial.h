#pragma once

#include "material/BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:
		VulkanBackendMaterial();

		~VulkanBackendMaterial() = default;

		void Init(const Ref<Pipeline> &shader) override;

		void Bind(const Ref<Pipeline> &shader) override;

		void BindTexture(uint32_t binding, const Ref<Texture> &texture) override;

		void Shutdown() override;

	private:
		vk::raii::Device &mDevice;
		std::vector<uint32_t> mUniformBufferBindings;
		vk::raii::DescriptorSets mDescriptorSets = VK_NULL_HANDLE;
	};
}