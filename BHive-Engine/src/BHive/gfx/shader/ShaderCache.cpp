#include "ShaderCache.h"
#include "gfx/RenderCommand.h"
#include "ShaderUtils.h"
#include "core/FileSystem.h"

namespace BHive
{
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
	void Serialize(A& ar, ShaderCache::MetaData& meta)
	{
		ar(meta.Hash, meta.Stages);
	}
	
	std::filesystem::path ShaderCache::GetShaderCacheDir(const std::string &name)
	{
		return ShaderUtils::GetCacheDirectory() / name;
	}

	std::filesystem::path ShaderCache::GetStageCachePath(const std::string &name, EShaderStage stage)
	{
		if (RenderCommand::GetRendererAPI() == RendererAPI::Vulkan)
			return GetShaderCacheDir( name) / (name + GetCacheVulkanFileExtension(stage));

		if (RenderCommand::GetRendererAPI() == RendererAPI::Opengl)
			return GetShaderCacheDir(name) / (name + GetCacheOpenglFileExtension(stage));

		ASSERT(false)
		return "";
	}

	std::filesystem::path ShaderCache::GetMetaPath(const std::string &name)
	{
		return GetShaderCacheDir(name) / (name + ".meta");
	}

	uint64_t ShaderCache::ComputeHash(const std::string &source)
	{
		return std::hash<std::string>{}(source);
	}

	ShaderCache::MetaData ShaderCache::LoadMeta(const std::string &name)
	{
		MetaData meta;

		auto meta_path = GetMetaPath(name);
		if (!std::filesystem::exists(meta_path))
			return meta;

		std::ifstream in(meta_path);
		cereal::JSONInputArchive ar(in);

		ar(meta);

		meta.Valid = true;
		return meta;
	}

	void ShaderCache::StoreMeta(const std::string &name, const MetaData &meta)
	{
		std::ofstream out(GetMetaPath(name));
		cereal::JSONOutputArchive ar(out);
		ar(meta);
	}


	bool ShaderCache::HasValidCache(const ShaderAsset &asset, const std::string &source)
	{
		auto meta = LoadMeta(asset.Name);
		if (!meta.Valid)
			return false;

		if (meta.Hash != ComputeHash(source))
			return false;

		if (meta.Stages.size() != asset.Stages.size())
			return false; 
		
		for (auto& [stage, _] : asset.Stages) 
		{
			if (std::find(meta.Stages.begin(), meta.Stages.end(), stage) == meta.Stages.end()) 
				return false; 
		}

		for (auto& [stage, _] : asset.Stages)
		{
			auto path = GetStageCachePath(asset.Name, stage);
			if (!std::filesystem::exists(path))
				return false;
		}

		return true;
	}

	void ShaderCache::LoadCache(ShaderAsset &asset)
	{
		for (auto& [stage, data] : asset.Stages)
		{
			auto path = GetStageCachePath(asset.Name, stage);
			FileSystem::ReadFile(path, data.Spirv);

			//reflect
			asset.Reflection[stage].Reflect(stage, data.Spirv);
		}

	}

	void ShaderCache::StoreCache(const ShaderAsset &asset, const std::string& source)
	{
		auto dir = GetShaderCacheDir(asset.Name);
		if (!std::filesystem::exists(dir))
			std::filesystem::create_directories(dir);

		//Write SPIR-V
		for (auto& [stage, data] : asset.Stages)
		{
			auto path = GetStageCachePath(asset.Name, stage);
			FileSystem::WriteFile(path, data.Spirv);
		}

		//write metadata
		MetaData meta;
		meta.Hash = ComputeHash(source);

		

		for (auto &[stage, _] : asset.Stages)
			meta.Stages.push_back(stage);


		StoreMeta(asset.Name, meta);
	}
} // namespace BHive