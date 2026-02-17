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

		void Add(const char *name, const Ref<ShaderProgram> &shader);

		Ref<ShaderProgram> Load(const std::filesystem::path &file);

		Ref<ShaderProgram> Get(const char *name);

		bool Contains(const std::string &name);

		void Clear();

		static ShaderManager &Get();

	private:
		Shaders mShaders;
	};
} // namespace BHive