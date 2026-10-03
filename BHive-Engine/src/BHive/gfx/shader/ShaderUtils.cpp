#include "ShaderUtils.h"
#include "core/FileSystem.h"
#include "gfx/Enumerations.h"
#include <string_view>

namespace BHive
{
	namespace
	{
		struct FIncludeExpansionContext
		{
			std::vector<std::filesystem::path> Dependencies;
			std::unordered_set<std::string> DependencyKeys;
			std::unordered_set<std::string> OnceFiles;
			std::unordered_set<std::string> ActiveFiles;
		};

		std::string_view Trim(std::string_view value)
		{
			while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())))
				value.remove_prefix(1);
			while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())))
				value.remove_suffix(1);
			return value;
		}

		bool IsPragmaOnce(std::string_view line)
		{
			line = Trim(line);
			if (!line.starts_with("#pragma"))
				return false;

			line = Trim(line.substr(7));
			if (!line.starts_with("once"))
				return false;

			line = Trim(line.substr(4));
			return line.empty() || line.starts_with("//") || line.starts_with("/*");
		}

		std::string ExpandIncludesRecursive(const std::string &source, const std::filesystem::path &requestingPath, FIncludeExpansionContext &context, bool rootSource = false)
		{
			const auto normalizedPath = requestingPath.lexically_normal();
			const auto sourceName = normalizedPath.generic_string();
			if (context.DependencyKeys.insert(sourceName).second)
				context.Dependencies.emplace_back(normalizedPath);

			bool pragmaOnce = false;
			{
				std::stringstream sourceLines(source);
				std::string sourceLine;
				while (std::getline(sourceLines, sourceLine))
				{
					if (IsPragmaOnce(sourceLine))
					{
						pragmaOnce = true;
						break;
					}
				}
			}
			if (pragmaOnce && context.OnceFiles.contains(sourceName))
				return {};

			if (!context.ActiveFiles.insert(sourceName).second)
			{
				LOG_ERROR("Shader include cycle detected at {}", sourceName);
				return {};
			}

			if (pragmaOnce)
				context.OnceFiles.insert(sourceName);

			std::stringstream input(source);
			std::stringstream output;
			std::string line;
			int lineNumber = 0;

			output << "#line 0 \"" << sourceName << "\"\n";
			while (std::getline(input, line))
			{
				const auto trimmedLine = Trim(line);
				if (rootSource && trimmedLine.starts_with("#type"))
					context.OnceFiles.clear();

				if (IsPragmaOnce(trimmedLine))
				{
					output << '\n';
				}
				else if (trimmedLine.starts_with("#include"))
				{
					auto start = line.find('<');
					if (start == std::string::npos)
						start = line.find('"');
					const char closingDelimiter = start != std::string::npos && line[start] == '"' ? '"' : '>';
					auto end = start == std::string::npos ? std::string::npos : line.find(closingDelimiter, start + 1);
					if (start == std::string::npos || end == std::string::npos)
					{
						LOG_ERROR("Malformed shader include directive in {}:{}", sourceName, lineNumber + 1);
						output << '\n';
					}
					else
					{
						const auto includeName = line.substr(start + 1, end - start - 1);
						const auto includePath = ShaderUtils::ResolveIncludePath(includeName, sourceName);
						std::string includeSource;
						if (includePath.empty() || !FileSystem::ReadFile(includePath, includeSource))
						{
							LOG_ERROR("Failed to read shader include {} from {}", includeName, sourceName);
						}
						else
						{
							output << ExpandIncludesRecursive(includeSource, includePath, context);
						}
						output << "#line " + std::to_string(lineNumber + 1) + " \"" + sourceName + "\"\n";
					}
				}
				else
				{
					output << line << '\n';
				}
				lineNumber++;
			}

			context.ActiveFiles.erase(sourceName);
			return output.str();
		}
	} // namespace

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
		return ENGINE_SHADER_PATH "/.cache/";
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
		return ExpandIncludesWithDependencies(source, requestingPath).Source;
	}

	ShaderUtils::ExpandedSource ShaderUtils::ExpandIncludesWithDependencies(const std::string &source, const std::string &requestingPath)
	{
		FIncludeExpansionContext context;
		auto expandedSource = ExpandIncludesRecursive(source, requestingPath, context, true);
		return {std::move(expandedSource), std::move(context.Dependencies)};
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
