#include "UUID.h"
#include <random>

namespace BHive
{
	static std::random_device sDevice;
	static std::mt19937_64 sEngine(sDevice());
	static std::uniform_int_distribution sDistribution;

	UUID::UUID()
		: mID(sDistribution(sEngine))
	{
	}

	UUID::UUID(uint64_t id)
		: mID(id)
	{
	}

	UUID::UUID(const UUID &uuid)
		: mID(uuid.mID)
	{
	}

	UUID &UUID::operator=(uint64_t rhs)
	{
		mID = rhs;
		return *this;
	}

	bool UUID::operator==(const UUID &rhs) const
	{
		return rhs.mID == mID;
	}
	bool UUID::operator!=(const UUID &rhs) const
	{
		return !(*this == rhs);
	}

	std::string UUID::to_string() const
	{
		return std::to_string(mID);
	}
}