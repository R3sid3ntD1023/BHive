#pragma once

#include <stdint.h>
#include <xhash>

namespace BHive
{
	struct FInputCommand
	{
		uint32_t InKey = 0;
		uint8_t InMod = 0;

		inline bool operator==(const FInputCommand &rhs) const { return InKey == rhs.InKey && InMod == rhs.InMod; }
		inline bool operator!=(const FInputCommand &rhs) const { return !(*this == rhs); }
	};
} // namespace BHive

namespace std
{
	template <>
	struct hash<BHive::FCommand>
	{
		size_t operator()(const BHive::FInputCommand &command) const { return hash<uint32_t>()(command.InKey) | hash<uint8_t>()(command.InMod); }
	};

} // namespace std
