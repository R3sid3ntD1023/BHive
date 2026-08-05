#include "Shader.h"
#include "shader/ShaderAsset.h"
#include "shader/ShaderCompiler.h"
#include "shader/ShaderUtils.h"
#include "core/FileSystem.h"
#include "shader/ShaderProgram.h"
#include "shader/ShaderCache.h"

namespace BHive
{
	Ref<ShaderProgram> Shader::Create(const std::filesystem::path &path)
	{
		auto asset = CreateRef<ShaderAsset>();
		asset->Name = path.stem().string();
		asset->SourcePath = path;

		// load source
		std::string source;
		if (!FileSystem::ReadFile(path, source))
		{
			LOG_ERROR("Failed to load shader file: {}", path.string());
			return nullptr;
		}

		auto expanded_source = ShaderUtils::ExpandIncludes(source, asset->SourcePath.string());
		const uint64_t newHash = ShaderCache::ComputeHash(expanded_source);
		const uint64_t oldHash = ShaderCache::GetStoredHash(asset->Name);

		auto source_stages = ShaderUtils::PreProcess(expanded_source);

		// detect stages from preprocess
		for (auto &[stage, code] : source_stages)
		{
			asset->Stages[stage].Code = code;
		}

		bool changed = (newHash != oldHash);

		// Cache Check
		if (!changed && ShaderCache::HasValidCache(*asset, expanded_source))
		{
			ShaderCache::LoadCache(*asset);
			return ShaderProgram::Create(asset);
		}

		ShaderCompiler compiler(path);
		compiler.Init();
		compiler.Compile(*asset);

		if (changed)
			ShaderCache::StoreCache(*asset, expanded_source);

		return ShaderProgram::Create(asset);
	}

	Ref<ShaderProgram> Shader::Create(const std::string &name, const std::string &vert, const std::string &frag)
	{
		auto asset = CreateRef<ShaderAsset>();
		asset->Name = name;

		asset->Stages[EShaderStage::Vertex].Code = vert;
		asset->Stages[EShaderStage::Fragment].Code = frag;

		ShaderCompiler compiler(name);
		compiler.Init();
		compiler.Compile(*asset);

		return ShaderProgram::Create(asset);
	}
} // namespace BHive