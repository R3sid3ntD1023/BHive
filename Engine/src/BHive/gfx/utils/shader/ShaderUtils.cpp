#include "ShaderUtils.h"
#include <glad/glad.h>

namespace BHive
{
	uint32_t ShaderUtils::GetAPIShaderStage(EShaderStage stage)
	{
		switch (stage)
		{
		case BHive::ShaderStage_Vertex:
			return GL_VERTEX_SHADER;
		case BHive::ShaderStage_Fragment:
			return GL_FRAGMENT_SHADER;
		case BHive::ShaderStage_Compute:
			return GL_COMPUTE_SHADER;
		case BHive::ShaderStage_Geometry:
			return GL_GEOMETRY_SHADER;
		default:
			break;
		}

		ASSERT(false, "Invalid Shader Stage");
		return 0;
	}

	uint32_t ShaderUtils::GetAPIShaderBit(EShaderStage stage)
	{
		switch (stage)
		{
		case BHive::ShaderStage_Vertex:
			return GL_VERTEX_SHADER_BIT;
		case BHive::ShaderStage_Fragment:
			return GL_FRAGMENT_SHADER_BIT;
		case BHive::ShaderStage_Compute:
			return GL_COMPUTE_SHADER_BIT;
		case BHive::ShaderStage_Geometry:
			return GL_GEOMETRY_SHADER_BIT;
		default:
			break;
		}

		ASSERT(false, "Invalid Shader Stage");
		return 0;
	}

	std::string ShaderUtils::ToString(EShaderStage stage)
	{
		switch (stage)
		{
		case BHive::ShaderStage_Vertex:
			return "Vertex Stage";
		case BHive::ShaderStage_Fragment:
			return "Fragment Stage";
		case BHive::ShaderStage_Compute:
			return "Compute Stage";
		case BHive::ShaderStage_Geometry:
			return "Geometry Stage";
		default:
			break;
		}

		ASSERT(false, "Invalid Shader Stage");
		return "";
	}

	EShaderStage ShaderUtils::FromString(const std::string &str)
	{
		if (str == "vertex")
			return ShaderStage_Vertex;
		if (str == "fragment")
			return ShaderStage_Fragment;
		if (str == "compute")
			return ShaderStage_Compute;
		if (str == "geometry")
			return ShaderStage_Geometry;

		ASSERT(false, "Invalid shader type string");
		return ShaderStage_None;
	}

	ShaderUtils::PreProcessData ShaderUtils::PreProcess(const std::string &source, const std::string &preprocessors)
	{
		PreProcessData data;

		auto token = "#type";
		auto version_token = "#version";

		auto token_length = strlen(token);
		auto version_length = strlen(version_token);

		auto token_pos = source.find(token, 0);
		if (token_pos == std::string::npos)
		{
			ASSERT(token_pos, "No shader type token found! Define a #type token for each shader stage")
		}

		auto pos = token_pos;

		while (pos != std::string::npos)
		{
			auto eol = source.find_first_of("\r\n", pos);
			ASSERT(eol != std::string::npos);
			auto begin = pos + token_length + 1;
			auto type = source.substr(begin, eol - begin);
			auto stage = ShaderUtils::FromString(type);
			ASSERT(stage);

			auto next_pos = source.find_first_not_of("\r\n", eol);
			ASSERT(next_pos != std::string::npos);
			pos = source.find(token, next_pos);

			auto code = (pos == std::string::npos) ? source.substr(next_pos) : source.substr(next_pos, pos - next_pos);
			{
				auto v_start = code.find_first_of(version_token);
				auto v_end = code.find_first_of('\r\n', v_start);
				code.insert(code.begin() + v_end, preprocessors.begin(), preprocessors.end());
			}

			data.emplace(stage, code);
		}

		return data;
	}

} // namespace BHive
