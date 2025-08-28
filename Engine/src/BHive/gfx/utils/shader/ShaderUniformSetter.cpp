#include "ShaderUniformSetter.h"
#include <glad/glad.h>

namespace BHive
{
	ShaderUniformSetter::ShaderUniformSetter(uint32_t shaderID)
		: mShaderID(shaderID)
	{
	}

	int ShaderUniformSetter::GetUniformLocation(const std::string &name) const
	{
		if (mUniformLocationCache.contains(name))
			return mUniformLocationCache.at(name);

		int location = glGetUniformLocation(mShaderID, name.c_str());
		mUniformLocationCache[name] = location;
		return location;
	}

	void ShaderUniformSetter::SetUniform(int location, int value) const
	{
		glProgramUniform1i(mShaderID, location, value);
	}

	void ShaderUniformSetter::SetUniform(int location, bool value) const
	{
		SetUniform(location, static_cast<uint32_t>(value));
	}

	void ShaderUniformSetter::SetUniform(int location, uint32_t value) const
	{
		glProgramUniform1ui(mShaderID, location, value);
	}

	void ShaderUniformSetter::SetUniform(int location, uint16_t value) const
	{
		SetUniform(location, static_cast<uint32_t>(value));
	}

	void ShaderUniformSetter::SetUniform(int location, float value) const
	{
		glProgramUniform1f(mShaderID, location, value);
	}

	void ShaderUniformSetter::SetUniform(int location, const glm::vec2 &value) const
	{
		glProgramUniform2fv(mShaderID, location, 1, &value.x);
	}

	void ShaderUniformSetter::SetUniform(int location, const glm::ivec2 &value) const
	{
		glProgramUniform2iv(mShaderID, location, 1, &value.x);
	}

	void ShaderUniformSetter::SetUniform(int location, const glm::vec3 &value) const
	{
		glProgramUniform3fv(mShaderID, location, 1, &value.x);
	}

	void ShaderUniformSetter::SetUniform(int location, const glm::vec4 &value) const
	{
		glProgramUniform4fv(mShaderID, location, 1, &value.x);
	}

	void ShaderUniformSetter::SetUniform(int location, const glm::mat4 &value) const
	{
		glProgramUniformMatrix4fv(mShaderID, location, 1, GL_FALSE, &value[0].x);
	}

	void ShaderUniformSetter::SetUniform(int location, uint64_t texture) const
	{
		glProgramUniformHandleui64NV(mShaderID, location, texture);
	}

} // namespace BHive