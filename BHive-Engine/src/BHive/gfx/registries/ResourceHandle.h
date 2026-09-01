#pragma once

#include "core/Core.h"
#include "core/type/TypeID.h"

namespace BHive
{
	struct BHIVE_API ResourceHandle
	{
		uint32_t Index = UINT32_MAX;
		uint32_t Generation = 0;
		uint32_t Type = 0;

		template <typename T>
		T *As() const
		{
			return static_cast<T *>(Resolve());
		}

		template <typename T>
		T &AsChecked() const
		{
			auto ptr = As<T>();
			ASSERT(ptr, "Invalid type cast");
			return *ptr;
		}

		template <typename T>
		bool Is() const
		{
			return As<T>() != nullptr;
		}

		bool IsValid() const { return Index != UINT32_MAX && Generation != 0; }

		operator bool() const { return IsValid(); }

		bool operator==(const ResourceHandle &other) const { return Index == other.Index && Generation == other.Generation && Type == other.Type; }

		bool operator!=(const ResourceHandle &other) const { return !(*this == other); }

		template <typename Ar>
		void Serialize(Ar &ar, uint32_t)
		{
			ar(Index, Generation, Type);
		}

	private:
		void *Resolve() const;
	};

} // namespace BHive

template <>
struct fmt::formatter<BHive::ResourceHandle> : fmt::formatter<std::string>
{
	using formatted_type = BHive::ResourceHandle;

	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const formatted_type &v, FormatContext &ctx) const
	{
		return fmt::format_to(ctx.out(), "ResourceHandle(Index: {}, Generation: {}, Type: {})", v.Index, v.Generation, v.Type);
	}
};
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
			uint64_t combined = (uint64_t(h.Index) << 32) ^ (uint64_t(h.Generation) << 16) ^ uint64_t(h.Type);
			return mix64(combined);
		}
	};
} // namespace std