#include "EventDelegateHandle.h"

namespace BHive
{
	EventDelegateHandle::EventDelegateHandle()
		: mID(UUID::Null)
	{
	}

	EventDelegateHandle::EventDelegateHandle(EGenerateNewHandle)
	{
		GenerateNewID();
	}

	void EventDelegateHandle::reset()
	{
		mID = UUID::Null;
	}

	bool EventDelegateHandle::operator==(const EventDelegateHandle &rhs) const
	{
		return mID == rhs.mID;
	}
	bool EventDelegateHandle::operator!=(const EventDelegateHandle &rhs) const
	{
		return mID == rhs.mID;
	}

	EventDelegateHandle::operator bool() const
	{
		return (uint64_t)mID != 0;
	}

	void EventDelegateHandle::GenerateNewID()
	{
		mID = UUID();
	}
} // namespace BHive