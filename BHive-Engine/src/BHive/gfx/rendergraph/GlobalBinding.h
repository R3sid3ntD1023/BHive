#pragma once

#include "core/Core.h"

namespace BHive
{
	struct GlobalBinding
	{
		uint32_t Set;
		uint32_t Binding;

		bool operator==(const GlobalBinding &rhs) const { return Set == rhs.Set && Binding == rhs.Binding; }
	};

} // namespace BHive

namespace std
{
	template <>
	struct hash<BHive::GlobalBinding>
	{
		size_t operator()(const BHive::GlobalBinding &binding) const { return std::hash<uint32_t>()(binding.Set) + std::hash<uint32_t>()(binding.Binding); }
	};
} // namespace std