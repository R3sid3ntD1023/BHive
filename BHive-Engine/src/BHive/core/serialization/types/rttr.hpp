#pragma once

#include <rttr/type.h>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <string>
#include <filesystem>
#include <unordered_map>

namespace rttr
{

	struct ReflectionRedirector
	{
		ReflectionRedirector(const std::filesystem::path &filename)
		{
			if (!std::filesystem::exists(filename))
			{
				std::ofstream out(filename);
				cereal::JSONOutputArchive ar(out);
				ar(cereal::make_nvp("RedirectedTypes", mRedirectedTypes));
				return;
			}

			std::ifstream in(filename);
			cereal::JSONInputArchive ar(in);
			ar(cereal::make_nvp("RedirectedTypes", mRedirectedTypes));
		}

		const std::string redirect(const std::string &value) const
		{
			if (mRedirectedTypes.contains(value))
				return mRedirectedTypes.at(value);

			return value;
		}

	private:
		std::unordered_map<std::string, std::string> mRedirectedTypes;
	};

	template <typename A>
	inline std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const type &obj)
	{
		return obj.get_name().data();
	}

	template <typename A>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, type &obj, const std::string &value)
	{
		static ReflectionRedirector redirector("redirected_classes.json");

		obj = type::get_by_name(redirector.redirect(value));
	}
} // namespace rttr