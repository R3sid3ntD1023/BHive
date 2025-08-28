#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include "ShaderStages.h"
#include "utils/shader/ShaderUniformSetter.h"
#include <glm/glm.hpp>

namespace BHive
{
	class ShaderBase
	{
	public:
		virtual ~ShaderBase() = default;

		template <typename T>
		void SetUniform(const std::string &name, const T &val)
		{
			if (GetSetter())
				GetSetter()->SetUniform<T>(name, val);
		}

		virtual uint32_t GetRendererID() const = 0;

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) {}

	protected:
		virtual ShaderUniformSetter *GetSetter() const = 0;

		struct FShaderData
		{
			std::string Code;
			std::vector<uint32_t> VulkanSpirv;
			std::vector<uint32_t> OpenglSpirv;
			std::string OpenglCompiledSource;
		};
	};

	class BHIVE_API Shader : public ShaderBase
	{
	public:
		Shader(const std::filesystem::path &path);

		virtual ~Shader();

		virtual void Bind() const;

		virtual void UnBind() const;

		virtual uint32_t GetRendererID() const override { return mProgramID; }

		virtual const std::string &GetName() const { return mName; }

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) override;

		ShaderUniformSetter *GetSetter() const override;

		virtual const FShaderReflectionData &GetRelectionData() const { return mReflectionData; }

		operator uint32_t() const { return GetRendererID(); }

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

		Scope<ShaderUniformSetter> mUniformSetter;
	};

	class PipelineShader : public ShaderBase
	{
	public:
		PipelineShader(const std::filesystem::path &path);

		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) override;

		ShaderUniformSetter *GetSetter() const override;

		virtual uint32_t GetRendererID() const override { return mShaderProgramID; }

	private:
		uint32_t mShaderProgramID{0};
		Scope<ShaderUniformSetter> mUniformSetter;
		std::unordered_map<EShaderStage, FShaderData> mSources;
	};

} // namespace BHive
