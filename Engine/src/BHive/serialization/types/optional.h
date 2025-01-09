#pragma once

#include <optional>

namespace BHive
{
	template <typename TArchive, typename T>
	inline void Deserialize(TArchive &ar, std::optional<T>& optional)
	{
		bool has_value;

		ar(has_value);
		if (has_value)
		{
			optional.emplace();
			ar(*optional);
		}
		else
		{
			optional = std::nullopt;
		}
	}

	template <typename TArchive, typename T>
	inline void Serialize(TArchive &ar, const std::optional<T> &optional)
	{
		if (optional)
		{
			ar(true);
			ar(*optional);
		}
		else
		{
			ar(false);
		}
		
	}
} // namespace BHive