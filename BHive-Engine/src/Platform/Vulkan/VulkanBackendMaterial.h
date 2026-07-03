#pragma once

#include "VulkanCore.h"
#include "gfx/material/BackendMaterial.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class ShaderProgram;
	struct FShaderReflection;
	class GPUBuffer;
	class VulkanPipeline;

	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:
		VulkanBackendMaterial();

		~VulkanBackendMaterial() = default;

		void Init(Pipeline* pipeline) override;

		void BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip , Pipeline* pipeline) override;

		void Set(const std::string &name, const void *data, size_t size) override;

		const FSetReflection &GetTargetSet() const override { return mTargetSet; }

		const FShaderReflection *GetRefl() const { return mReflectionMergedPtr; }

		MaterialSnapshot CreateSnapshot() const;

	private:
		vk::raii::Device &mDevice;

		vk::PipelineBindPoint mBindPoint = vk::PipelineBindPoint::eGraphics;

		Ref<ShaderProgram> mProgram;
		
		const FShaderReflection *mReflectionMergedPtr = nullptr;

		const FShaderReflectionLookUp *mReflectionLookupTablePtr = nullptr;

		FSetReflection mTargetSet;

		std::vector<std::byte> mPushConstantData;

		std::unordered_map<std::string, Ref<GPUBuffer>> mLocalBuffers;

		std::unordered_map<std::string, MaterialSnapshot::TextureBinding> mTextureBindings;

	};
}