#include "EventDelegateHandle.h"

namespace BHive
{
    EventDelegateHandle::EventDelegateHandle()
        : mID(0)
    {
    }

    EventDelegateHandle::EventDelegateHandle(EGenerateNewHandle)
    {
        GenerateNewID();
    }

    void EventDelegateHandle::reset() { mID = 0; }

    bool EventDelegateHandle::operator==(const EventDelegateHandle &rhs) const { return mID == rhs.mID; }
    bool EventDelegateHandle::operator!=(const EventDelegateHandle &rhs) const { return mID == rhs.mID; }

    EventDelegateHandle::operator bool() const { return (uint64_t)mID != 0; }

    void EventDelegateHandle::GenerateNewID()
    {
        mID = UUID();
    }
}