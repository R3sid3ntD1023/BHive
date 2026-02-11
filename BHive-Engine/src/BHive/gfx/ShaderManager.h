#pragma once

#include "core/Core.h"
#include "Shader.h"

namespace BHive
{
	struct ShaderMetaData
	{
		std::filesystem::path Path;
	};

	class BHIVE_API ShaderManager
	{
	public:
		ShaderManager();
		ShaderManager(const ShaderManager &) = delete;

		using Shaders = std::unordered_map<std::string, Ref<Shader>>;
		using ShaderMetaDatas = std::unordered_map<std::string, ShaderMetaData>;

		void Add(const char *name, const Ref<Shader> &shader);

		Ref<Shader> Load(const std::filesystem::path &file, const Shader::FRenderOptions &options = {});

		Ref<Shader> Get(const char *name);

		bool Contains(const std::string &name);

		void Clear();

		static ShaderManager &Get();

	private:
		Shaders mShaders;
		ShaderMetaDatas mShaderMetaDatas;

		static inline Scope<ShaderManager> sInstance = nullptr;
	};
} // namespace BHive