#pragma once

#include "core/Core.h"
#include <stdint.h>
#include <string>
#include <xhash>

namespace BHive
{
	struct NullID_t
	{
		explicit constexpr NullID_t() = default;
	};

	inline constexpr NullID_t NullID{};

	class BHIVE_API UUID
	{
	public:
		UUID();
		UUID(uint64_t high, uint64_t low);
		UUID(const UUID &) = default;
		UUID(NullID_t);

		bool IsValid() const { return mHigh != 0ULL && mLow != 0ULL; }

		uint64_t Hash() const { return mHigh ^ (mLow * 0x93779b97f4a7c15ULL); };

		std::string ToString() const;

		UUID &FromString(const std::string &s);

		auto operator<=>(const UUID &rhs) const = default;

		operator uint64_t() const { return Hash(); }

		operator bool() const { return IsValid(); }

		template <typename Ar>
		std::string SaveMinimal(const Ar &ar) const
		{
			return ToString();
		}

		template <typename Ar>
		void LoadMinimal(const Ar &ar, const std::string &v)
		{
			FromString(v);
		}

	private:
		uint64_t mHigh = 0;
		uint64_t mLow = 0;

		friend struct std::hash<BHive::UUID>;
	};

} // namespace BHive

namespace std
{

	template <>
	struct hash<BHive::UUID>
	{
		size_t operator()(const BHive::UUID &uuid) const { return std::hash<uint64_t>()(uuid.mHigh) ^ std::hash<uint64_t>()(uuid.mLow) << 1; }
	};
} // namespace std