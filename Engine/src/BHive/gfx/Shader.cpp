#include "Shader.h"
#include "Platform/GL/GLShader.h"

namespace BHive
{
	void Shader::SetBindlessTexture(const std::string &name, uint64_t texture) const
	{
		auto location = GetUniformLocation(name);
		SetBindlessTexture(location, texture);
	}

	Ref<Shader> Shader::Create(const std::filesystem::path &path)
	{
		return CreateRef<GLShader>(path);
	}

	Ref<Shader> Shader::Create(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader)
	{
		return CreateRef<GLShader>(name, vertex_shader, fragment_shader);
	}

	void ShaderLibrary::Add(const char *name, const Ref<Shader> &shader)
	{
		if (!Contains(name))
		{
			mShaders.emplace(name, shader);
		}
	}

	Ref<Shader> ShaderLibrary::Load(const char *name, const std::string &vertex_src, const std::string &fragment_src)
	{
		if (!Contains(name))
		{
			auto shader = Shader::Create(name, vertex_src, fragment_src);
			auto operation = mShaders.emplace(name, shader);
			return (*operation.first).second;
		}

		return mShaders.at(name);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path &file)
	{
		auto name = file.filename().string();
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		auto &shader = mShaders[name];
		shader = Shader::Create(file);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const char *name)
	{
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		return {};
	}

	bool ShaderLibrary::Contains(const std::string &name)
	{
		return mShaders.contains(name);
	}

} // namespace BHive