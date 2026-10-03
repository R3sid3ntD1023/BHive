#include "ShaderFactory.h"
#include "Platform/Vulkan/VulkanResourceSet.h"
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
		return Create(path, {}, {});
	}

	ShaderPtr ShaderFactory::Create(const std::filesystem::path &path, const std::vector<std::string> &definitions, const std::string &variantName)
	{
		auto resolvedPath = FileSystem::ResolvePath(path, ENGINE_SHADER_PATH);

		ShaderAsset asset{};
		asset.CacheName = path.stem().string();
		asset.VariantName = variantName;
		asset.Name = asset.CacheName;
		if (!variantName.empty())
			asset.Name += "_" + variantName;
		asset.SourcePath = path;

		// load source
		std::string source;
		if (!FileSystem::ReadFile(resolvedPath, source))
		{
			LOG_ERROR("Failed to load shader file: {}", path.string());
			return {};
		}

		auto expanded = ShaderUtils::ExpandIncludesWithDependencies(source, asset.SourcePath.string());
		auto &expanded_source = expanded.Source;
		asset.Dependencies = std::move(expanded.Dependencies);
		const uint64_t newHash = ShaderCache::ComputeHash(expanded_source);
		const uint64_t oldHash = ShaderCache::GetStoredHash(asset);

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
		MacroDefinitions macroDefinitions;
		for (const auto &[stage, _] : asset.Stages)
			macroDefinitions.Names[stage] = definitions;
		compiler.Compile(asset, macroDefinitions);

		if (changed)
			ShaderCache::StoreCache(asset, expanded_source);

		return Create(asset);
	}

	ShaderPtr ShaderFactory::Create(const std::string &name, const std::string &vert, const std::string &frag)
	{
		ShaderAsset asset{};
		asset.Name = name;
		asset.CacheName = name;

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

	ResourceSetPtr ResourceSetFactory::Create(const BindingSetTemplate &setTemplate)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateResource<VulkanResourceSet>(setTemplate);
		}

		ASSERT(false)
		return {};
	}

} // namespace BHive