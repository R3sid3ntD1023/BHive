#pragma once

#include "core/Core.h"
#include "ShaderReflection.h"
#include <glm/glm.hpp>

namespace BHive
{

	class BHIVE Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Compile() = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

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

		virtual uint32_t GetRendererID() const = 0;
		virtual const std::string &GetName() const = 0;
		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1) = 0;
		virtual const FShaderReflectionData &GetRelectionData() const = 0;

		operator uint32_t() const { return GetRendererID(); }

		static Ref<Shader> Create(const std::filesystem::path &path);
		static Ref<Shader>
		Create(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader);
		static Ref<Shader> Create(const std::string &name, const std::string &comp_shader);

	private:
		virtual void SetUniform(int location, int value) const = 0;
		virtual void SetUniform(int location, uint32_t value) const = 0;
		virtual void SetUniform(int location, float value) const = 0;
		virtual void SetUniform(int location, const glm::vec2 &value) const = 0;
		virtual void SetUniform(int location, const glm::ivec2 &value) const = 0;
		virtual void SetUniform(int location, const glm::vec3 &value) const = 0;
		virtual void SetUniform(int location, const glm::vec4 &value) const = 0;
		virtual void SetUniform(int location, const glm::mat4 &value) const = 0;
		virtual void SetUniform(int location, uint64_t value) const = 0;
		virtual int GetUniformLocation(const std::string &name) const = 0;
	};

	namespace utils
	{
		bool SetIncludeFromFile(const std::string &name, const char *filename);
	}
} // namespace BHive
