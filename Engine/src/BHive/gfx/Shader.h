#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include "ShaderStages.h"
#include "utils/shader/ShaderUniformSetter.h"
#include "VulkanCore.h"

namespace BHive
{
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
		void SetUniform(const std::string &name, const T &val)
		{
			if (mUniformSetter)
				mUniformSetter->SetUniform<T>(name, val);
		}

		virtual const FShaderReflectionData &GetRelectionData() const { return mReflectionData; }

		operator uint32_t() const { return GetRendererID(); }

		void Save(cereal::BinaryOutputArchive &ar) const;

		void Load(cereal::BinaryInputArchive &ar);

		const FShaderData &GetShaderData(EShaderStage stage) const;

		const Stages &GetStageCreateInfos() const { return mVulkanShaderStages; }

	private:
		void Compile();

		void CompileFromSource();

		void PreProcess(const std::string &source);

		void Reflect();

	private:
		std::string mName;

		uint32_t mProgramID{0};

		FShaderReflectionData mReflectionData;

		std::filesystem::path mFilePath;

		std::unordered_map<EShaderStage, FShaderData> mSources;

		std::vector<vk::raii::ShaderModule> mVulkanShaderModules{};

		Stages mVulkanShaderStages{};

		Scope<ShaderUniformSetter> mUniformSetter;

		friend class ShaderSerializer;
	};

} // namespace BHive
