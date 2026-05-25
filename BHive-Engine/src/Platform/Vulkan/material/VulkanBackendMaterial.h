#pragma once

#include "Platform/Vulkan/VulkanCore.h"
#include "gfx/material/BackendMaterial.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class ShaderProgram;
	struct FShaderReflection;
	class GPUBuffer;
	class ISetManager;

	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:
		VulkanBackendMaterial();

		~VulkanBackendMaterial() = default;

		void Init(const Ref<Pipeline> &shader) override;

		void Bind(const Ref<Pipeline> &shader) override;

		void BindImmediate(vk::raii::CommandBuffer &buffer, const Ref<Pipeline> &pipeline);

		void BindTextureImmediate(const std::string &name, const Ref<Texture> &texture, uint32_t mip, const Ref<Pipeline> &pipeline);

		void BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip , const Ref<Pipeline>& pipeline) override;

		void Set(const std::string &name, const void *data, size_t size) override;

		void Shutdown() override;

		const FSetReflection &GetTargetSet() const { return mTargetSet; }

	private:
		vk::raii::Device &mDevice;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		Ref<ShaderProgram> mProgram;
		
		std::vector<uint8_t> mPushConstantData;

		std::unordered_map<std::string, Ref<GPUBuffer>> mLocalBuffers;

		const FShaderReflection* mReflectionMergedPtr = nullptr;

		const FShaderReflectionLookUp *mReflectionLookupTablePtr = nullptr; 

		FSetReflection mTargetSet;

		friend class MaterialSetRegistry;
	};
}