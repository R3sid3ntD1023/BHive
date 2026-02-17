#pragma once

#include "core/Core.h"
#include "shader/ShaderReflection.h"

namespace BHive
{
	class Texture;
	class ShaderProgram;
	
	class BHIVE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual const std::string &GetName() const = 0;

		virtual const FShaderReflection &GetRefl() const = 0;

		static Ref<ShaderProgram> Create(const std::filesystem::path &path);

		static Ref<ShaderProgram> Create(const std::string &name, const std::string &vert, const std::string &frag);

	};

} // namespace BHive

