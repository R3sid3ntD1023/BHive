#include "UUID.h"
#include "Rpc.h"
#pragma comment(lib, "Rpcrt4.lib")

namespace BHive
{

	UUID::UUID()
	{
		static const int uuid_string_length = 36;
		::GUID id;
		RPC_CSTR uuid_string = nullptr;

		auto result = CoCreateGuid(&id);
		if (SUCCEEDED(result))
		{
			result = (result == RPC_S_UUID_LOCAL_ONLY) ? S_OK : result;

			if (SUCCEEDED(result))
				result = ::UuidToString(&id, &uuid_string);

			if (SUCCEEDED(result))
			{
				mGUID.resize(uuid_string_length);
				memcpy(&mGUID[0], uuid_string, uuid_string_length);
			}

			if (uuid_string != nullptr)
			{
				RpcStringFree(&uuid_string);
			}
		}
	}

	UUID::UUID(const char *guid)
		: mGUID(guid)
	{
	}

	UUID::UUID(const std::string &guid)
		: mGUID(guid)
	{
	}

	UUID::UUID(NullID_t null)
		: mGUID(null)
	{
	}

	UUID &UUID::operator=(const char *rhs)
	{
		mGUID = rhs;
		return *this;
	}

	UUID &UUID::operator=(NullID_t null)
	{
		mGUID = null;
		return *this;
	}

	bool UUID::operator==(const UUID &rhs) const
	{
		return rhs.mGUID == mGUID;
	}

	bool UUID::operator!=(const UUID &rhs) const
	{
		return !(*this == rhs);
	}
} // namespace BHive