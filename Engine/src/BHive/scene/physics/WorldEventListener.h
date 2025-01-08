#pragma once

#include "physics/PhysicsCore.h"
#include "core/EventDelegate.h"

DECLARE_EVENT(FOnContact, const rp3d::CollisionCallback::ContactPair&)
DECLARE_EVENT(FOnTrigger, const rp3d::OverlapCallback::OverlapPair&)
DECLARE_EVENT(FOnHit, const rp3d::RaycastInfo&)

namespace BHive
{
    class CollisionEventListener : public rp3d::EventListener
    {
    public:
        virtual void onContact(const rp3d::CollisionCallback::CallbackData &callbackData) override;

        virtual void onTrigger(const rp3d::OverlapCallback::CallbackData &callbackData) override;

        FOnContactEvent OnContact;
        FOnTriggerEvent OnTrigger;
    };

    class HitEventListener : public rp3d::RaycastCallback
    {
	public:
		virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo &info) override;

        FOnHitEvent OnHit;
    };

} // namespace BHive
