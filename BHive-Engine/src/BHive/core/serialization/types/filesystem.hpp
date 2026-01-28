#pragma once

#include <cereal/cereal.hpp>
#include <filesystem>

namespace std::filesystem
{
	template <typename A>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const path &path)
	{
		return path.string();
	}

	template <typename A>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, path &path, const std::string &value)
	{
		path = value;
	}

} // namespace std::filesystem