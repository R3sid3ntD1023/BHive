#include "Shader.h"
#include "Platform/GL/GLShader.h"

namespace BHive
{

	Ref<Shader> Shader::Create(const std::filesystem::path &path)
	{
		return CreateRef<GLShader>(path);
	}

	Ref<Shader> Shader::Create(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader)
	{
		return CreateRef<GLShader>(name, vertex_shader, fragment_shader);
	}

	Ref<Shader> Shader::Create(const std::string &name, const std::string &comp_shader)
	{
		return CreateRef<GLShader>(name, comp_shader);
	}

} // namespace BHive