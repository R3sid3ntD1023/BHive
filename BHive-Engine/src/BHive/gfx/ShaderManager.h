#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderProgram.h"


namespace BHive
{
	class BHIVE_API ShaderManager
	{
	public:
		ShaderManager();
		ShaderManager(const ShaderManager &) = delete;

		using Shaders = std::unordered_map<std::string, Ref<ShaderProgram>>;

		static void Register(const char *name, const Ref<ShaderProgram> &shader);

		static Ref<ShaderProgram> Load(const std::filesystem::path &file);

		static Ref<ShaderProgram> Get(const char *name);

		static bool Contains(const std::string &name);

		static void Clear();

	private:
		static inline Shaders mShaders;
	};
} // namespace BHive