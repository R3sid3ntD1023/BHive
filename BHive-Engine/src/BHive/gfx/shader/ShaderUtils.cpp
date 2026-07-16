#include "ShaderUtils.h"
#include "gfx/Enumerations.h"
#include "core/FileSystem.h"

namespace BHive
{
	std::string ShaderUtils::ToString(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:
			return "Vertex Stage";
		case EShaderStage::Fragment:
			return "Fragment Stage";
		case EShaderStage::Compute:
			return "Compute Stage";
		case EShaderStage::Geometry:
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
			return EShaderStage::Vertex;
		if (str == "fragment")
			return EShaderStage::Fragment;
		if (str == "compute")
			return EShaderStage::Compute;
		if (str == "geometry")
			return EShaderStage::Geometry;

		ASSERT(false, "Invalid shader type string");
		return EShaderStage::None;
	}

	std::filesystem::path ShaderUtils::GetCacheDirectory()
	{
		return ENGINE_SHADER_PATH "/cache/";
	}

	ShaderUtils::PreProcessData ShaderUtils::PreProcess(const std::string &source, const std::string &preprocessors)
	{
		PreProcessData data;

		auto token = "#type";
		auto version_token = "#version";
		auto token_length = strlen(token);

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
			auto stage = FromString(type);
			ASSERT(stage != EShaderStage::None);

			auto next_pos = source.find_first_not_of("\r\n", eol);
			ASSERT(next_pos != std::string::npos);
			pos = source.find(token, next_pos);

			auto code = (pos == std::string::npos) ? source.substr(next_pos) : source.substr(next_pos, pos - next_pos);
			{
				auto v_start = code.find_first_of(version_token);
				auto v_end = code.find_first_of("\r\n", v_start);
				code.insert(code.begin() + v_end, preprocessors.begin(), preprocessors.end());
			}

			data.emplace(stage, code);
		}

		return data;
	}

	std::string ShaderUtils::ExpandIncludes(const std::string &source, const std::string &requestingPath)
	{
		std::stringstream input(source);
		std::stringstream output;

		std::string line;
		int lineNumber = 0;

		output << "#line 0 \"" << requestingPath + "\"\n";

		while (std::getline(input, line))
		{
			if (line.starts_with("#include"))
			{
				auto start = line.find("<") + 1;
				auto end = line.find(">", start);

				std::string includeName = line.substr(start, end - start);
				std::filesystem::path includePath = ResolveIncludePath(includeName, requestingPath);

				std::string includeSource;
				FileSystem::ReadFile(includePath.string(), includeSource);

				output << ExpandIncludes(includeSource, includePath.string());
				output << "#line " + std::to_string(lineNumber + 1) + " \"" + requestingPath + "\"\n";
			}
			else
			{
				output << line << "\n";
			}

			lineNumber++;
		}

		return output.str();
	}

	std::string ShaderUtils::ResolveIncludePath(const std::string &requested, const std::string &requesting)
	{
		std::filesystem::path directory = std::filesystem::path(requesting).parent_path();
		std::filesystem::path resolved_path = directory / requested;

		// use default engine path, if file isn't relative
		if (!std::filesystem::exists(resolved_path))
		{
			std::filesystem::recursive_directory_iterator it(ENGINE_SHADER_PATH);
			for (auto &entry : it)
			{
				if (entry.path().filename() == requested)
				{
					resolved_path = entry;
					break;
				}
			}
		}

		if (!std::filesystem::exists(resolved_path))
		{
			LOG_ERROR("ShaderIncluder::ERROR - Failed to find file : {} requsted from {}", requested, requesting);
			return "";
		}

		return resolved_path.string();
	}
} // namespace BHive
