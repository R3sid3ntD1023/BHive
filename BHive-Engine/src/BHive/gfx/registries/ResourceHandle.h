#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ResourceHandle
	{
		uint32_t Index = UINT32_MAX;
		uint32_t Generation = 0;
		uint64_t Type = 0;
		void *Ptr = nullptr;

		template <typename T>
		T *As() const
		{
			return reinterpret_cast<T *>(Ptr);
		}

		bool IsValid() const { return Index != UINT32_MAX && Generation != 0 && Type != 0; }

		bool operator==(const ResourceHandle &h) const { return Index == h.Index && Generation == h.Generation && Type == h.Type; }

		operator bool() const { return IsValid(); }

		template <typename Ar>
		void Serialize(Ar &ar, uint32_t)
		{
			ar(Index, Generation, Type);
		}
	};
} // namespace BHive

namespace std
{
	// Murmur‑style
	static inline uint64_t mix64(uint64_t x)
	{
		x ^= x >> 33;
		x *= 0xff51afd7ed558ccdULL;
		x ^= x >> 33;
		x *= 0xc4ceb9fe1a85ec53ULL;
		x ^= x >> 33;
		return x;
	}

	template <>
	struct hash<BHive::ResourceHandle>
	{
		size_t operator()(const BHive::ResourceHandle &h) const
		{
			uint64_t combined = (uint64_t(h.Index) << 32) ^ (uint64_t(h.Generation) << 16) ^ h.Type;
			return mix64(combined);
		}
	};
} // namespace std