#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include <glm/glm.hpp>

namespace BHive
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path &path);
		Shader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader);
		Shader(const std::string &name, const std::string &comp_shader);

		virtual ~Shader();

		virtual void Compile();
		virtual void Bind() const;
		virtual void UnBind() const;

		template <typename T>
		void SetUniform(const std::string &name, const T &v) const
		{
			int location = GetUniformLocation(name);
			if (location == -1)
			{
				LOG_ERROR("Failed to find Uniform {} for {}", name, GetName());
				return;
			}

			SetUniform(location, v);
		}

		virtual uint32_t GetRendererID() const { return mShaderID; }
		virtual const std::string &GetName() const { return mName; }
		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1);
		virtual const FShaderReflectionData &GetRelectionData() const { return mReflectionData; }

		operator uint32_t() const { return GetRendererID(); }

	private:
		virtual void SetUniform(int location, int value) const;
		virtual void SetUniform(int location, uint32_t value) const;
		virtual void SetUniform(int location, float value) const;
		virtual void SetUniform(int location, const glm::vec2 &value) const;
		virtual void SetUniform(int location, const glm::ivec2 &value) const;
		virtual void SetUniform(int location, const glm::vec3 &value) const;
		virtual void SetUniform(int location, const glm::vec4 &value) const;
		virtual void SetUniform(int location, const glm::mat4 &value) const;
		virtual void SetUniform(int location, uint64_t value) const;
		virtual int GetUniformLocation(const std::string &name) const;
		void PreProcess(const std::string &source);
		void Reflect();

	private:
		std::string mName;
		std::unordered_map<uint32_t, std::string> mSources;
		uint32_t mShaderID{0};
		FShaderReflectionData mReflectionData;
		mutable std::unordered_map<std::string, int> mUniformLocationCache;
	};

	namespace utils
	{
		bool SetIncludeFromFile(const std::string &name, const char *filename);
	}
} // namespace BHive
