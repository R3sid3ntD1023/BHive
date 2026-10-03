#include "ShaderCache.h"
#include "ShaderUtils.h"
#include "core/FileSystem.h"
#include "gfx/RenderCommand.h"
#include <cctype>

namespace BHive
{
	namespace
	{
		std::string GetSafeVariantFolder(const std::string &variantName)
		{
			std::string folder;
			folder.reserve(variantName.size());
			for (const unsigned char character : variantName)
				folder += std::isalnum(character) || character == '_' || character == '-' ? static_cast<char>(character) : '_';
			return folder.empty() ? "unnamed" : folder;
		}
	} // namespace

	const char *GetCacheOpenglFileExtension(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:
			return ".cached_opengl.vert";
		case EShaderStage::Fragment:
			return ".cached_opengl.frag";
		case EShaderStage::Compute:
			return ".cached_opengl.comp";
		case EShaderStage::Geometry:
			return ".cached_opengl.geom";

		default:
			break;
		}
		ASSERT(false)
		return "";
	}

	const char *GetCacheVulkanFileExtension(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:
			return ".cached_vulkan.vert";
		case EShaderStage::Fragment:
			return ".cached_vulkan.frag";
		case EShaderStage::Compute:
			return ".cached_vulkan.comp";
		case EShaderStage::Geometry:
			return ".cached_vulkan.geom";
		default:
			break;
		}
		ASSERT(false)
		return "";
	}

	template <typename A>
	void Serialize(A &ar, ShaderCache::MetaData &meta)
	{
		ar(meta.Hash, meta.Stages, meta.MergedReflection);
	}

	std::filesystem::path ShaderCache::GetShaderCacheDir(const ShaderAsset &asset)
	{
		auto directory = ShaderUtils::GetCacheDirectory() / asset.CacheName;
		if (!asset.VariantName.empty())
			directory /= std::filesystem::path("variants") / GetSafeVariantFolder(asset.VariantName);
		return directory;
	}

	std::filesystem::path ShaderCache::GetStageCachePath(const ShaderAsset &asset, EShaderStage stage)
	{
		if (RenderCommand::GetAPI() == RendererAPI::Vulkan)
			return GetShaderCacheDir(asset) / (asset.Name + GetCacheVulkanFileExtension(stage));

		if (RenderCommand::GetAPI() == RendererAPI::Opengl)
			return GetShaderCacheDir(asset) / (asset.Name + GetCacheOpenglFileExtension(stage));

		ASSERT(false)
		return "";
	}

	std::filesystem::path ShaderCache::GetMetaPath(const ShaderAsset &asset)
	{
		return GetShaderCacheDir(asset) / (asset.Name + ".meta");
	}

	uint64_t ShaderCache::ComputeHash(const std::string &source)
	{
		return std::hash<std::string>{}(source);
	}

	ShaderCache::MetaData ShaderCache::LoadMeta(const ShaderAsset &asset)
	{
		MetaData meta;

		auto meta_path = GetMetaPath(asset);
		if (!std::filesystem::exists(meta_path))
			return meta;

		std::ifstream in(meta_path, std::ios::in | std::ios::binary);
		cereal::JSONInputArchive ar(in);

		ar(meta);

		meta.Valid = true;
		return meta;
	}

	void ShaderCache::StoreMeta(const ShaderAsset &asset, const MetaData &meta)
	{
		std::ofstream out(GetMetaPath(asset), std::ios::out | std::ios::binary);
		cereal::JSONOutputArchive ar(out);
		ar(meta);
	}

	bool ShaderCache::HasValidCache(const ShaderAsset &asset, const std::string &source)
	{
		auto meta = LoadMeta(asset);
		if (!meta.Valid)
			return false;

		if (meta.Hash != ComputeHash(source))
			return false;

		if (meta.Stages.size() != asset.Stages.size())
			return false;

		for (auto &[stage, _] : asset.Stages)
		{
			if (std::find(meta.Stages.begin(), meta.Stages.end(), stage) == meta.Stages.end())
				return false;
		}

		for (auto &[stage, _] : asset.Stages)
		{
			auto path = GetStageCachePath(asset, stage);
			if (!std::filesystem::exists(path))
				return false;
		}

		return true;
	}

	void ShaderCache::LoadCache(ShaderAsset &asset)
	{
		auto meta = LoadMeta(asset);

		for (auto &[stage, data] : asset.Stages)
		{
			auto path = GetStageCachePath(asset, stage);
			FileSystem::ReadFile(path, data.Spirv);
		}

		asset.MergedReflection = meta.MergedReflection;
	}

	void ShaderCache::StoreCache(const ShaderAsset &asset, const std::string &source)
	{
		auto dir = GetShaderCacheDir(asset);
		if (!std::filesystem::exists(dir))
			std::filesystem::create_directories(dir);

		// Write SPIR-V
		for (auto &[stage, data] : asset.Stages)
		{
			auto path = GetStageCachePath(asset, stage);
			FileSystem::WriteFile(path, data.Spirv);
		}

		// write metadata
		MetaData meta;
		meta.Hash = ComputeHash(source);

		for (auto &[stage, _] : asset.Stages)
			meta.Stages.push_back(stage);

		meta.MergedReflection = asset.MergedReflection;

		StoreMeta(asset, meta);
	}

	uint64_t ShaderCache::GetStoredHash(const ShaderAsset &asset)
	{
		auto meta = LoadMeta(asset);
		return meta.Valid ? meta.Hash : 0;
	}
} // namespace BHive