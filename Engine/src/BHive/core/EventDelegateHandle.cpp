#include "EventDelegateHandle.h"

namespace BHive
{
	EventDelegateHandle::EventDelegateHandle(EGenerateNewHandle)
	{
		GenerateNewID();
	}

	void EventDelegateHandle::reset()
	{
		mID = NullID;
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
		return (bool)mID;
	}

	void EventDelegateHandle::GenerateNewID()
	{
		mID = UUID();
	}
} // namespace BHive