#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class BHIVE_API ShaderManager
	{
	public:
		static ShaderPtr Load(const std::filesystem::path &file);

		static ShaderPtr Get(const std::string &name);

	private:
		static bool Contains(uint64_t hash) { return mShaders.contains(hash); }

	private:
		static inline std::unordered_map<uint64_t, ShaderPtr> mShaders;
	};
} // namespace BHive