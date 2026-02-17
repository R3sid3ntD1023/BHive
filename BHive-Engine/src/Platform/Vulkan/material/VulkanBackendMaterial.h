#pragma once

#include "material/BackendMaterial.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class ShaderProgram;

	struct BufferBinding
	{
		uint32_t Set;
		uint32_t Binding;
	};

	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:
		VulkanBackendMaterial();

		~VulkanBackendMaterial() = default;

		void Init(const Ref<Pipeline> &shader) override;

		void Bind(const Ref<Pipeline> &shader) override;

		void BindTexture(const std::string& name, const Ref<Texture> &texture) override;

		void Set(const std::string &name, const void *data, size_t size) override;

		void Shutdown() override;

	private:
		vk::raii::Device &mDevice;
		std::vector<vk::raii::DescriptorSets> mDescriptorSets;
		Ref<ShaderProgram> mProgram;
		const FShaderReflection *mReflectionPtr = nullptr; 
		std::vector<uint8_t> mPushConstantData;
	};
}