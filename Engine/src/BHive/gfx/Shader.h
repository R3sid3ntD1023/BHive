#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include "ShaderStages.h"
#include "VulkanCore.h"

namespace BHive
{
	class VulkanPipeline;
	class FDescriptorSetLayout;

	class BHIVE_API Shader
	{
		using Stages = std::vector<vk::PipelineShaderStageCreateInfo>;

		struct FShaderData
		{
			std::string Code;
			std::vector<uint32_t> VulkanSpirv;
			vk::PipelineShaderStageCreateInfo VulkanShaderStageInfo{};

			template <typename A>
			void Serialize(A &ar)
			{
				ar(Code, VulkanSpirv);
			}
		};

	public:
		Shader(const std::filesystem::path &path);

		virtual ~Shader();

		virtual void Bind() const;

		virtual void UnBind() const;

		virtual uint32_t GetRendererID() const { return mProgramID; }

		virtual const std::string &GetName() const { return mName; }

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1);

		template <typename T>
		void SetUniform(const std::string &name, const T &val);

		template <typename T, glm::length_t L, glm::qualifier Q>
		void SetUniform(const std::string &name, const glm::vec<L, T, Q> &val);

		template <typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
		void SetUniform(const std::string &name, const glm::mat<C, R, T, Q> &val);

		virtual const FShaderReflectionData &GetRelectionData() const { return mReflectionData; }

		operator uint32_t() const { return GetRendererID(); }

		void Save(cereal::BinaryOutputArchive &ar) const;

		void Load(cereal::BinaryInputArchive &ar);

		const FShaderData &GetShaderData(EShaderStage stage) const;

		const Stages &GetStageCreateInfos() const { return mVulkanShaderStages; }

		const VulkanPipeline &GetPipeline() const { return *mGraphicsPipeline; }

		const vk::raii::PipelineLayout &GetPipelineLayout() const { return mPipelineLayout; }

		const Ref<FDescriptorSetLayout> &GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

	private:
		void Compile();

		void CompileFromSource();

		void PreProcess(const std::string &source);

		void Reflect();

		void CreatePipeline();

	private:
		std::string mName;

		uint32_t mProgramID{0};

		FShaderReflectionData mReflectionData;

		std::filesystem::path mFilePath;

		std::unordered_map<EShaderStage, FShaderData> mSources;

		Stages mVulkanShaderStages{};

		Scope<VulkanPipeline> mGraphicsPipeline;

		Ref<FDescriptorSetLayout> mDescriptorSetLayout;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		friend class ShaderSerializer;
	};



} // namespace BHive

#include "Shader.inl"