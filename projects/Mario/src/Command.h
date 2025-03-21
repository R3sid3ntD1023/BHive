#pragma once

#include <xhash>
#include <stdint.h>

namespace BHive
{
	struct FCommand
	{
		int32_t InKey = 0;
		int32_t InMod = 0;

		inline bool operator==(const FCommand &rhs) const { return InKey == rhs.InKey && InMod == rhs.InMod; }
		inline bool operator!=(const FCommand &rhs) const { return !(*this == rhs); }
	};
} // namespace BHive

namespace std
{
	template <>
	struct hash<BHive::FCommand>
	{
		size_t operator()(const BHive::FCommand &command) const
		{
			return hash<int32_t>()(command.InKey) + hash<int32_t>()(command.InMod);
		}
	};

} // namespace std
