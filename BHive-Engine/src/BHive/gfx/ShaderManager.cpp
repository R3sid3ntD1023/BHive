#include "core/subsystem/SubSystem.h"
#include "Shader.h"
#include "ShaderManager.h"

namespace BHive
{
	ShaderManager::ShaderManager()
	{
		
	}

	void ShaderManager::Register(const char *name, const Ref<ShaderProgram> &shader)
	{
		if (!Contains(name))
		{
			mShaders.emplace(name, shader);
		}
	}

	Ref<ShaderProgram> ShaderManager::Load(const std::filesystem::path &file)
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

		//creates shader program (compiles + reflects internally)
		auto program = Shader::Create(resolved_path);
		mShaders[name] = program;

		return program;
	}

	Ref<ShaderProgram> ShaderManager::Get(const char *name)
	{
		if (Contains(name))
		{
			return mShaders.at(name);
		}

		std::filesystem::recursive_directory_iterator directory(ENGINE_SHADER_PATH);
		for (auto &entry : directory)
		{
			auto filename = entry.path().filename().string();
			if (filename == name)
			{
				return Load(entry.path());
			}
		}

		return {};
	}


	bool ShaderManager::Contains(const std::string &name)
	{
		return mShaders.contains(name);
	}

	void ShaderManager::Clear()
	{
		mShaders.clear();
	}


} // namespace BHive