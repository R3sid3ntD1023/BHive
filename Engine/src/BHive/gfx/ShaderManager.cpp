#include "core/subsystem/SubSystem.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "utils/shader/ShaderTimeCache.h"

namespace BHive
{
	ShaderManager::ShaderManager()
	{
		AddSubSystem<ShaderTimeCache>();
	}

	void ShaderManager::Add(const char *name, const Ref<Shader> &shader)
	{
		if (!Contains(name))
		{
			mShaders.emplace(name, shader);
		}
	}

	Ref<Shader> ShaderManager::Load(const std::filesystem::path &file)
	{
		std::filesystem::path resolved_path = file;
		if (!file.is_absolute())
		{
			std::filesystem::recursive_directory_iterator directory(ENGINE_SHADER_PATH);
			for (auto &entry : directory)
			{
				auto filename = entry.path().string();

				if (filename.find(file.string()) != std::string::npos)
				{
					resolved_path = entry;
					break;
				}
			}
		}

		auto name = resolved_path.stem().string();
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		auto &shader = mShaders[name];
		mShaderMetaDatas[name].Path = resolved_path;

		shader = CreateRef<Shader>(resolved_path);
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