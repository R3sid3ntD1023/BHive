#include "ShaderManager.h"
#include "Shader.h"
#include "core/FileSystem.h"
#include "core/utils/Hash.h"
#include "gfx/factories/ShaderFactory.h"

namespace BHive
{
	ShaderPtr ShaderManager::Load(const std::filesystem::path &path)
	{
		auto name = path.stem().string();
		auto hash = utils::ComputeHash(name);

		if (!Contains(hash))
		{
			// creates shader program (compiles + reflects internally)
			auto program = ShaderFactory::Create(path);
			if (program)
			{
				mShaders[hash] = program;
			}
		}

		return mShaders.at(hash);
	}

	ShaderPtr ShaderManager::Get(const std::string &name)
	{
		auto hash = utils::ComputeHash(name);
		if (Contains(hash))
		{

			return mShaders.at(hash);
		}

		auto resolvedPath = FileSystem::ResolvePath(name, ENGINE_SHADER_PATH);
		if (std::filesystem::exists(resolvedPath))
		{
			return Load(resolvedPath);
		}

		return {};
	}
} // namespace BHive