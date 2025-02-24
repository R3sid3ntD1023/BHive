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

		virtual const std::string &GetName() const { return mName; }
		uint32_t GetRendererID() const { return mShaderID; }
		void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1);

	private:
		virtual void SetUniform(int location, int value) const;
		virtual void SetUniform(int location, uint32_t value) const;
		virtual void SetUniform(int location, float value) const;
		virtual void SetUniform(int location, const glm::vec2 &value) const;
		virtual void SetUniform(int location, const glm::ivec2 &value) const;
		virtual void SetUniform(int location, const glm::vec3 &value) const;
		virtual void SetUniform(int location, const glm::vec4 &value) const;
		virtual void SetUniform(int location, const glm::mat4 &value) const;
		virtual void SetBindlessTexture(int location, uint64_t texture) const;
		virtual int GetUniformLocation(const std::string &name) const;
		void PreProcess(const std::string &source);
		void Reflect();

	private:
		std::string mName;
		std::unordered_map<uint32_t, std::string> mSources;
		uint32_t mShaderID{0};
		ShaderRelfectionData mReflectionData;
		mutable std::unordered_map<std::string, int> mUniformLocationCache;
	};
} // namespace BHive