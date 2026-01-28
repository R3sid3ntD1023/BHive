#pragma once

#include "core/Core.h"
#include <stdint.h>
#include <string>
#include <xhash>

namespace BHive
{
	struct NullID_t
	{
		operator std::string() const { return "00000000-0000-0000-0000-000000000000"; }
		bool operator==(const std::string &rhs) const { return rhs == "00000000-0000-0000-0000-000000000000"; }
	};

	inline constexpr NullID_t NullID{};

	class BHIVE_API UUID
	{
	public:
		UUID();
		UUID(const char *guid);
		UUID(const std::string &guid);
		UUID(const UUID &) = default;
		UUID(NullID_t);

		UUID &operator=(const char *rhs);
		UUID &operator=(NullID_t);

		bool operator==(const UUID &rhs) const;
		bool operator!=(const UUID &rhs) const;

		operator const std::string &() const { return mGUID; }
		operator bool() const { return mGUID != NullID; }

		template <typename Ar>
		std::string SaveMinimal(const Ar &ar) const
		{
			return mGUID;
		}

		template <typename Ar>
		void LoadMinimal(const Ar &ar, const std::string &v)
		{
			mGUID = v;
		}

	private:
		std::string mGUID;

		friend struct std::hash<BHive::UUID>;
	};

} // namespace BHive

namespace std
{

	template <>
	struct hash<BHive::UUID>
	{
		size_t operator()(const BHive::UUID &uuid) const { return std::hash<std::string>()(uuid.mGUID); }
	};
} // namespace std