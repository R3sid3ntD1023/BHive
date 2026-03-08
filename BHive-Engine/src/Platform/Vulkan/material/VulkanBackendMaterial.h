#pragma once

#include "Platform/Vulkan/VulkanCore.h"
#include "material/BackendMaterial.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class ShaderProgram;
	struct FShaderReflection;
	class UniformBuffer;
	class StorageBuffer;
	class ISetManager;

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

		const FSetReflection &GetTargetSet() const { return mTargetSet; }

	private:
		vk::raii::Device &mDevice;

		Ref<ISetManager> mMaterialSetManager;

		Ref<ShaderProgram> mProgram;
		
		std::vector<uint8_t> mPushConstantData;

		std::unordered_map<std::string, Ref<UniformBuffer>> mLocalUBOs;

		std::unordered_map<std::string, Ref<StorageBuffer>> mLocalSSBOs;

		const FShaderReflection* mReflectionMergedPtr = nullptr;

		const FShaderReflectionLookUp *mReflectionLookupTablePtr = nullptr; 

		FSetReflection mTargetSet;
	};
}