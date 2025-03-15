#pragma once

#include <stdint.h>
#include <string>
#include <xhash>

namespace BHive
{

	class UUID
	{
	public:
		UUID();
		UUID(const char *guid);
		UUID(const std::string &guid);
		UUID(const UUID &) = default;

		UUID &operator=(const char *rhs);

		bool operator==(const UUID &rhs) const;
		bool operator!=(const UUID &rhs) const;

		operator const std::string &() const { return mGUID; }
		operator bool() const { return mGUID != Null.mGUID; }

		static UUID Null;

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
		std::string mGUID = UUID::Null;

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