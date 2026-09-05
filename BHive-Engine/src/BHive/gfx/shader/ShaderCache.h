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

		static uint64_t GetStoredHash(const std::string &name);

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
		static std::filesystem::path GetShaderCacheDir(const std::string &name);

		static std::filesystem::path GetStageCachePath(const std::string &name, EShaderStage stage);

		static std::filesystem::path GetMetaPath(const std::string &name);

		static MetaData LoadMeta(const std::string &name);

		static void StoreMeta(const std::string &name, const MetaData &meta);
	};
} // namespace BHive