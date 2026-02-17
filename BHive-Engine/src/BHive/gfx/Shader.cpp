#include "Shader.h"
#include "shader/ShaderAsset.h" 
#include "shader/ShaderCompiler.h" 
#include "shader/ShaderUtils.h"
#include "core/FileSystem.h"
#include "shader/ShaderProgram.h"
#include "shader/ShaderTimeCache.h"

namespace BHive
{
	Ref<ShaderProgram> Shader::Create(const std::filesystem::path &path)
	{
		auto asset = CreateRef<ShaderAsset>();
		asset->Name = path.stem().string();
		asset->SourcePath = path;

		//load source
		std::string source;
		if (!FileSystem::ReadFile(path, source))
		{
			LOG_ERROR("Failed to load shader file: {}", path.string());
			return nullptr;
		}

		auto source_stages = ShaderUtils::PreProcess(source);

		//detect stages from preprocess
		for (auto& [stage, code] : source_stages)
		{
			asset->Stages[stage].Code = code;
		}

		//Cache Check
		if (ShaderCache::HasValidCache(*asset, source))
		{
			ShaderCache::LoadCache(*asset);
			return CreateRef<ShaderProgram>(asset);
		}

		ShaderCompiler compiler(path);
		compiler.Init();
		compiler.Compile(*asset);

		ShaderCache::StoreCache(*asset, source);

		return CreateRef<ShaderProgram>(asset);
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

		return CreateRef<ShaderProgram>(asset);
	}
} // namespace BHive