#include "UUID.h"
#include "Rpc.h"
#pragma comment(lib, "Rpcrt4.lib")

namespace BHive
{

	UUID::UUID()
	{
		::GUID guid;
		auto result = CoCreateGuid(&guid);
		if (SUCCEEDED(result))
		{
			mHigh = uint64_t(guid.Data1 << 32) | uint64_t(guid.Data2 << 16) | guid.Data3;
			mLow = uint64_t(guid.Data4[0] << 56) | uint64_t(guid.Data4[1] << 48) | uint64_t(guid.Data4[2] << 40) | uint64_t(guid.Data4[3] << 32) | uint64_t(guid.Data4[4] << 24)
				   | uint64_t(guid.Data4[5] << 16) | uint64_t(guid.Data4[6] << 8) | guid.Data4[7];
		}
	}

	UUID::UUID(uint64_t high, uint64_t low)
		: mHigh(high),
		  mLow(low)
	{
	}

	UUID::UUID(NullID_t null)
		: mHigh(0),
		  mLow(0)
	{
	}

	std::string UUID::ToString() const
	{
		char buffer[37];

		std::snprintf(
			buffer,
			sizeof(buffer),
			"%08x-%04x-%04x-%04x-%012llx",
			uint32_t(mHigh >> 32),
			uint16_t(mHigh >> 16),
			uint16_t(mHigh),
			uint16_t(mLow >> 48),
			uint64_t(mLow & 0x0000FFFFFFFFFFFFULL)
		);
		return buffer;
	}

	UUID &UUID::FromString(const std::string &s)
	{
		uint32_t a;
		uint16_t b;
		uint16_t c;
		uint16_t d;
		uint64_t e;

		std::sscanf(s.c_str(), "%8x-%4hx-%4hx-%4hx-%12llx", &a, &b, &c, &d, &e);
		mHigh = uint64_t(a << 32) | uint64_t(b << 16) | c;
		mLow = uint64_t(d << 48) | e;
		return *this;
	}
} // namespace BHive