#pragma once

#include "core/Core.h"
#include "gfx/Shader.h"
#include "VulkanBackend.h"

namespace BHive
{
	class Pipeline;
	class FDescriptorSetLayout;
	class FDescriptorPool;

	using ShaderModules = std::unordered_map<EShaderStage, vk::raii::ShaderModule> ;

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

		virtual const std::string &GetName() const override { return mName; }

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) override;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

		const ShaderModules &GetModules() const { return mShaderModules; }

		void Reflect()  override;

		const FShaderReflectionData &GetRefl() const override { return mRefl; }

	private:
		void Compile();

		void CompileFromSource();

		void PreProcess(const std::string &source);

		void CreateDescriptorResources();

	private:
		vk::raii::Device &mDevice;

		ShaderModules mShaderModules;

		vk::raii::DescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;

		std::filesystem::path mFilePath;

		std::string mName;

		std::unordered_map<EShaderStage, FShaderData> mSources;

		FShaderReflectionData mRefl;
	};

	

} // namespace BHive