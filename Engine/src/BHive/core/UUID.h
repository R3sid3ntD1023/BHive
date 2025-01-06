#pragma once

#include <stdint.h>
#include <xhash>
#include "serialization/Serialization.h"

namespace BHive
{
	class StreamReader;
	class StreamWriter;

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

		void Serialize(StreamWriter &stream) const;

		void Deserialize(StreamReader &ar);

		std::string to_string() const;

	protected:
		uint64_t mID{0};

		friend struct std::hash<BHive::UUID>;
	};
}

namespace std
{
	template <>
	struct hash<BHive::UUID>
	{
		size_t operator()(const BHive::UUID &uuid) const
		{
			return uuid.mID;
		}
	};
}