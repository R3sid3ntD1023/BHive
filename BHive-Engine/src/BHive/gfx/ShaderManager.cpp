#include "ShaderManager.h"
#include "core/FileSystem.h"
#include "core/utils/Hash.h"
#include "gfx/factories/ShaderFactory.h"
#include "shader/Shader.h"

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

	ShaderPtr ShaderManager::GetVariant(const std::string &name, std::vector<std::string> definitions)
	{
		if (definitions.empty())
			return Get(name);

		std::sort(definitions.begin(), definitions.end());
		definitions.erase(std::unique(definitions.begin(), definitions.end()), definitions.end());

		std::string variantName;
		for (const auto &definition : definitions)
		{
			if (!variantName.empty())
				variantName += '_';
			variantName += definition;
		}

		const auto key = name + '_' + variantName;
		const auto hash = utils::ComputeHash(key);
		if (Contains(hash))
			return mShaders.at(hash);

		auto resolvedPath = FileSystem::ResolvePath(name, ENGINE_SHADER_PATH);
		if (!std::filesystem::exists(resolvedPath))
			return {};

		auto program = ShaderFactory::Create(resolvedPath, definitions, variantName);
		if (program)
			mShaders[hash] = program;
		return program;
	}
} // namespace BHive