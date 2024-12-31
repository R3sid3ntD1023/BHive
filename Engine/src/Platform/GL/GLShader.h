#pragma once

#include "gfx/Shader.h"

#define REFLECT_GL_SHADERS 0

namespace BHive
{
	class GLShader : public Shader
	{
	public:
		GLShader(const std::filesystem::path &path);
		GLShader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader);
		~GLShader();

		void Compile();
		void Bind() const;
		void UnBind() const;
		void SetUniform(const std::string &name, int value) const;
		void SetUniform(const std::string &name, uint32_t value) const;
		void SetUniform(const std::string &name, float value) const;
		void SetUniform(const std::string &name, const glm::vec2 &value) const;
		void SetUniform(const std::string &name, const glm::vec3 &value) const;
		void SetUniform(const std::string &name, const glm::vec4 &value) const;
		void SetUniform(const std::string &name, const glm::mat4 &value) const;
		void SetUniform(const std::string &name, uint64_t handle) const;

		uint32_t GetRendererID() const { return mShaderID; }
		void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1);

	private:
		uint32_t GetUniformLocation(const std::string &name) const;
		void PreProcess(const std::string &source);
		void Reflect();

	private:
		std::string mName;
		std::unordered_map<uint32_t, std::string> mSources;
		uint32_t mShaderID{0};
		ShaderRelfectionData mReflectionData;
	};
}