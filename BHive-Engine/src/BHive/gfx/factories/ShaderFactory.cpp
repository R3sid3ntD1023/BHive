#include "ShaderFactory.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "core/FileSystem.h"
#include "gfx/RenderCommand.h"
#include "gfx/shader/Shader.h"
#include "gfx/shader/ShaderCache.h"
#include "gfx/shader/ShaderCompiler.h"
#include "gfx/shader/ShaderUtils.h"

namespace BHive
{
	ShaderPtr ShaderFactory::Create(const std::filesystem::path &path)
	{
		auto resolvedPath = FileSystem::ResolvePath(path, ENGINE_SHADER_PATH);

		ShaderAsset asset{};
		asset.Name = path.stem().string();
		asset.SourcePath = path;

		// load source
		std::string source;
		if (!FileSystem::ReadFile(resolvedPath, source))
		{
			LOG_ERROR("Failed to load shader file: {}", path.string());
			return {};
		}

		auto expanded_source = ShaderUtils::ExpandIncludes(source, asset.SourcePath.string());
		const uint64_t newHash = ShaderCache::ComputeHash(expanded_source);
		const uint64_t oldHash = ShaderCache::GetStoredHash(asset.Name);

		auto source_stages = ShaderUtils::PreProcess(expanded_source);

		// detect stages from preprocess
		for (auto &[stage, code] : source_stages)
		{
			asset.Stages[stage].Code = code;
		}

		bool changed = (newHash != oldHash);

		// Cache Check
		if (!changed && ShaderCache::HasValidCache(asset, expanded_source))
		{
			ShaderCache::LoadCache(asset);
			return Create(asset);
		}

		ShaderCompiler compiler(path);
		compiler.Init();
		compiler.Compile(asset);

		if (changed)
			ShaderCache::StoreCache(asset, expanded_source);

		return Create(asset);
	}

	ShaderPtr ShaderFactory::Create(const std::string &name, const std::string &vert, const std::string &frag)
	{
		ShaderAsset asset{};
		asset.Name = name;

		asset.Stages[EShaderStage::Vertex].Code = vert;
		asset.Stages[EShaderStage::Fragment].Code = frag;

		ShaderCompiler compiler(name);
		compiler.Init();
		compiler.Compile(asset);

		return Create(asset);
	}

	ShaderPtr ShaderFactory::Create(const ShaderAsset &asset)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateResource<VulkanShader>(asset);
		}

		ASSERT(false, "Unsupported RendererAPI");
		return {};
	}

} // namespace BHive