#pragma once

#include "core/Core.h"

namespace BHive
{
	class Shader;

	struct ShaderMetaData
	{
		std::filesystem::path Path;
	};

	class ShaderManager
	{
	public:
		ShaderManager() = default;
		ShaderManager(const ShaderManager &) = delete;

		using Shaders = std::unordered_map<std::string, Ref<Shader>>;
		using ShaderMetaDatas = std::unordered_map<std::string, ShaderMetaData>;

		void LoadFiles(const std::filesystem::path &directory);

		void Add(const char *name, const Ref<Shader> &shader);

		Ref<Shader> Load(const char *name, const std::string &vertex_src, const std::string &fragment_src);

		Ref<Shader> Load(const char *name, const std::string &comp_src);

		Ref<Shader> Load(const std::filesystem::path &file);

		Ref<Shader> Get(const char *name);

		bool Contains(const std::string &name);

		static ShaderManager &Get();

	private:
		Shaders mShaders;
		ShaderMetaDatas mShaderMetaDatas;

		static inline Scope<ShaderManager> sInstance = nullptr;
	};
} // namespace BHive