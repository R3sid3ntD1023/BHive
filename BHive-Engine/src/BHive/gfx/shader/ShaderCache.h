#pragma once

#include "ShaderAsset.h"
#include "core/Core.h"

namespace BHive
{
	class ShaderCache
	{
	public:
		static bool HasValidCache(const ShaderAsset &asset, const std::string &source);

		static void LoadCache(ShaderAsset &asset);

		static void StoreCache(const ShaderAsset &asset, const std::string &source);

		static uint64_t GetStoredHash(const ShaderAsset &asset);

		static uint64_t ComputeHash(const std::string &source);

	private:
		struct MetaData
		{
			uint64_t Hash = 0;
			std::vector<EShaderStage> Stages;
			bool Valid = false;
			FShaderReflection MergedReflection;
		};

	private:
		static std::filesystem::path GetShaderCacheDir(const ShaderAsset &asset);

		static std::filesystem::path GetStageCachePath(const ShaderAsset &asset, EShaderStage stage);

		static std::filesystem::path GetMetaPath(const ShaderAsset &asset);

		static MetaData LoadMeta(const ShaderAsset &asset);

		static void StoreMeta(const ShaderAsset &asset, const MetaData &meta);
	};
} // namespace BHive