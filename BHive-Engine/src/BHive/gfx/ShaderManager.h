#pragma once

#include "core/Core.h"
#include "Shader.h"
#include "ShaderReflection.h"


namespace BHive
{
	class BHIVE_API ShaderManager
	{
	public:
		ShaderManager();
		ShaderManager(const ShaderManager &) = delete;

		using Shaders = std::unordered_map<std::string, Ref<Shader>>;

		void Add(const char *name, const Ref<Shader> &shader);

		Ref<Shader> Load(const std::filesystem::path &file);

		Ref<Shader> Get(const char *name);

		bool Contains(const std::string &name);

		void Clear();

		static ShaderManager &Get();

	private:
		Shaders mShaders;
	};
} // namespace BHive