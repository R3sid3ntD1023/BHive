#pragma once

#include "physics/PhysicsCore.h"
#include "core/EventDelegate.h"

DECLARE_EVENT(FOnContact, rp3d::CollisionCallback::ContactPair)
DECLARE_EVENT(FOnTrigger, rp3d::OverlapCallback::OverlapPair)

namespace BHive
{
    class WorldEventListener : public rp3d::EventListener
    {
    public:
        virtual void onContact(const rp3d::CollisionCallback::CallbackData &callbackData) override;

        virtual void onTrigger(const rp3d::OverlapCallback::CallbackData &callbackData) override;

        FOnContactEvent OnContact;
        FOnTriggerEvent OnTrigger;
    };

} // namespace BHive
