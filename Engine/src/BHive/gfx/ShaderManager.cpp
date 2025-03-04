#include "ShaderManager.h"
#include "Shader.h"

#define SHADER_EXTENSION ".glsl"

namespace BHive
{
	void ShaderManager::LoadFiles(const std::filesystem::path &directory)
	{
		if (!std::filesystem::exists(directory))
		{
			ASSERT(false, directory);
		}

		for (auto &entry : std::filesystem::recursive_directory_iterator(directory))
		{
			auto path = entry.path();
			auto extension = path.extension().string();

			if (extension == SHADER_EXTENSION)
			{
				Load(path);
				LOG_INFO("Loaded shader file {}", path);
			}
		}
	}

	void ShaderManager::Add(const char *name, const Ref<Shader> &shader)
	{
		if (!Contains(name))
		{
			mShaders.emplace(name, shader);
		}
	}

	Ref<Shader> ShaderManager::Load(const char *name, const std::string &vertex_src, const std::string &fragment_src)
	{
		if (!Contains(name))
		{
			auto shader = CreateRef<Shader>(name, vertex_src, fragment_src);
			auto operation = mShaders.emplace(name, shader);
			return (*operation.first).second;
		}

		return mShaders.at(name);
	}

	BHIVE Ref<Shader> ShaderManager::Load(const char *name, const std::string &comp_src)
	{
		if (!Contains(name))
		{
			auto shader = CreateRef<Shader>(name, comp_src);
			auto operation = mShaders.emplace(name, shader);
			return (*operation.first).second;
		}

		return mShaders.at(name);
	}

	Ref<Shader> ShaderManager::Load(const std::filesystem::path &file)
	{
		auto name = file.filename().string();
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		auto &shader = mShaders[name];
		mShaderMetaDatas[name].Path = file;

		shader = CreateRef<Shader>(file);
		return shader;
	}

	Ref<Shader> ShaderManager::Get(const char *name)
	{
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		return {};
	}

	bool ShaderManager::Contains(const std::string &name)
	{
		return mShaders.contains(name);
	}

	ShaderManager &ShaderManager::Get()
	{
		if (!sInstance)
		{
			sInstance = CreateScope<ShaderManager>();
		}

		return *sInstance;
	}
} // namespace BHive