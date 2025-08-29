#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include "ShaderStages.h"
#include "utils/shader/ShaderUniformSetter.h"

namespace BHive
{
	class BHIVE_API Shader
	{

		struct FShaderData
		{
			std::string Code;
			std::vector<uint32_t> VulkanSpirv;
			std::vector<uint32_t> OpenglSpirv;
			std::string OpenglCompiledSource;

			template <typename A>
			void Serialize(A &ar)
			{
				ar(Code, VulkanSpirv, OpenglSpirv, OpenglCompiledSource);
			}
		};

	public:
		Shader(const std::filesystem::path &path);

		virtual ~Shader();

		virtual void Bind() const;

		virtual void UnBind() const;

		virtual uint32_t GetRendererID() const { return mProgramID; }

		uint32_t GetSeperableProgram(EShaderStage stage) const;

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

	private:
		void Compile();

		void PreProcess(const std::string &source);

		void Reflect();

	private:
		std::string mName;

		uint32_t mProgramID{0};

		FShaderReflectionData mReflectionData;

		std::filesystem::path mFilePath;

		std::unordered_map<EShaderStage, FShaderData> mSources;

		std::unordered_map<EShaderStage, uint32_t> mSeperablePrograms;

		Scope<ShaderUniformSetter> mUniformSetter;

		friend class ShaderSerializer;
	};

} // namespace BHive
