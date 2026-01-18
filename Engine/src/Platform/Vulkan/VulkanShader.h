#pragma once

#include "core/Core.h"
#include "gfx/Shader.h"
#include "gfx/ShaderReflection.h"
#include "VulkanCore.h"

namespace BHive
{
	class Pipeline;
	class FDescriptorSetLayout;
	class FDescriptorPool;

	class BHIVE_API VulkanShader : public Shader
	{
		struct FShaderData
		{
			std::string Code;
			std::vector<uint32_t> VulkanSpirv;

			template <typename A>
			void Serialize(A &ar)
			{
				ar(Code, VulkanSpirv);
			}
		};

	public:
		VulkanShader(const std::filesystem::path &path);

		VulkanShader(const std::string &name, const std::string &vert, const std::string &frag);

		virtual ~VulkanShader();

		virtual void Bind() override;

		virtual void UnBind() override;

		virtual const std::string &GetName() const override { return mFilePath.stem().string(); }

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) override;

		virtual void BindTexture(uint32_t binding, const Ref<Texture> &texture) override;

		virtual const FShaderReflectionData &GetRelectionData() const override { return mReflectionData; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		const vk::raii::PipelineLayout &GetPipelineLayout() const { return mPipelineLayout; }

		const vk::raii::DescriptorSets &GetDescriptorSets() const { return mDescriptorSets; }

		const Ref<Pipeline> &GetPipeline() const { return mGraphicsPipeline; }

	private:
		void Compile();

		void CompileFromSource();

		void PreProcess(const std::string &source);

		void Reflect();

		void CreatePipeline();

		void CreateDescriptorResources();

		void DestroyDescriptorResources();

		void UpdateDescriptorResources();

	private:
		vk::raii::Device &mDevice;

		Ref<Pipeline> mGraphicsPipeline;

		Ref<FDescriptorSetLayout> mDescriptorSetLayout;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		std::unordered_map<EShaderStage, vk::raii::ShaderModule> mShaderModules;

		Ref<FDescriptorPool> mDescriptorPool;

		vk::raii::DescriptorSets mDescriptorSets = VK_NULL_HANDLE;

		std::vector<uint32_t> mUniformBufferBindings;

		std::filesystem::path mFilePath;

		FShaderReflectionData mReflectionData;

		std::unordered_map<EShaderStage, FShaderData> mSources;
	};

} // namespace BHive