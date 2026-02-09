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
		VulkanShader(const std::filesystem::path &path, const FRenderOptions &options = {});

		VulkanShader(const std::string &name, const std::string &vert, const std::string &frag, const FRenderOptions &options = {});

		virtual ~VulkanShader();

		virtual void Bind() override;

		virtual void UnBind() override;

		virtual const std::string &GetName() const override { return mName; }

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) override;

		virtual const FShaderReflectionData &GetRelectionData() const override { return mReflectionData; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		const vk::raii::PipelineLayout &GetPipelineLayout() const { return mPipelineLayout; }

		const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

		const Ref<Pipeline> &GetPipeline() const { return mGraphicsPipeline; }

	private:
		void Compile();

		void CompileFromSource();

		void PreProcess(const std::string &source);

		void Reflect();

		void CreatePipeline();

		void CreateDescriptorResources();


	private:
		vk::raii::Device &mDevice;

		Ref<Pipeline> mGraphicsPipeline;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		std::unordered_map<EShaderStage, vk::raii::ShaderModule> mShaderModules;

		vk::raii::DescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;


		std::filesystem::path mFilePath;

		std::string mName;

		FShaderReflectionData mReflectionData;

		std::unordered_map<EShaderStage, FShaderData> mSources;

		FRenderOptions mRenderOptions;
	};

	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:

		VulkanBackendMaterial();

		void Init(const Ref<Shader> &shader) override;

		void Bind(const Ref<Shader> &shader) override;

		void BindTexture(uint32_t binding, const Ref<Texture> &texture) override;

	private:		
		vk::raii::Device &mDevice;
		std::vector<uint32_t> mUniformBufferBindings;
		std::unordered_map < uint32_t, Ref<Texture>> mTextures;
		vk::raii::DescriptorSets mDescriptorSets = VK_NULL_HANDLE;

	};

} // namespace BHive