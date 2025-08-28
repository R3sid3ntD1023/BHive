#pragma once

#include "core/math/Math.h"

namespace BHive
{

	struct ShaderUniformSetter
	{
		ShaderUniformSetter(uint32_t shaderID);

		template <typename T>
		void SetUniform(const std::string &name, const T &val)
		{
			if (int location = GetUniformLocation(name); location != -1)
			{
				SetUniform(location, val);
			}
		}

	private:
		virtual int GetUniformLocation(const std::string &name) const;

		virtual void SetUniform(int location, int value) const;

		virtual void SetUniform(int location, bool value) const;

		virtual void SetUniform(int location, uint32_t value) const;

		virtual void SetUniform(int location, uint16_t value) const;

		virtual void SetUniform(int location, uint64_t value) const;

		virtual void SetUniform(int location, float value) const;

		virtual void SetUniform(int location, const glm::vec2 &value) const;

		virtual void SetUniform(int location, const glm::ivec2 &value) const;

		virtual void SetUniform(int location, const glm::vec3 &value) const;

		virtual void SetUniform(int location, const glm::vec4 &value) const;

		virtual void SetUniform(int location, const glm::mat4 &value) const;

	private:
		mutable std::unordered_map<std::string, int> mUniformLocationCache;

		uint32_t mShaderID{0};
	};
} // namespace BHive