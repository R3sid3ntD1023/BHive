#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderProgram.h"

namespace BHive
{
	class BHIVE_API ShaderManager
	{
	public:
		using Shaders = std::unordered_map<std::string, Ref<ShaderProgram>>;

		void Register(const std::string &name, const Ref<ShaderProgram> &shader);

		Ref<ShaderProgram> Load(const std::filesystem::path &file);

		Ref<ShaderProgram> Get(const std::string &name);

		bool Contains(const std::string &name);

		void Clear();

	private:
		Shaders mShaders;
	};
} // namespace BHive