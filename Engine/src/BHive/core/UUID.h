#pragma once

#include <stdint.h>
#include <xhash>
#include <string>

namespace BHive
{

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t id);
		UUID(const UUID &uuid);

		UUID &operator=(uint64_t rhs);

		bool operator==(const UUID &rhs) const;
		bool operator!=(const UUID &rhs) const;

		operator uint64_t() const { return mID; }

		std::string to_string() const;

	protected:
		uint64_t mID{0};

		friend struct std::hash<BHive::UUID>;
	};
} // namespace BHive

namespace std
{
	template <>
	struct hash<BHive::UUID>
	{
		size_t operator()(const BHive::UUID &uuid) const { return uuid.mID; }
	};
} // namespace std