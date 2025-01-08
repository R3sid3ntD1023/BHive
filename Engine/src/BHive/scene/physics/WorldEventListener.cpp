#include "WorldEventListener.h"

namespace BHive
{
	void CollisionEventListener::onContact(
		const rp3d::CollisionCallback::CallbackData &callbackData)
    {
        for (unsigned p = 0; p < callbackData.getNbContactPairs(); p++)
        {
            auto contactpair = callbackData.getContactPair(p);
            OnContact.invoke(contactpair);
        }
    }

    void CollisionEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData &callbackData)
    {
        for (unsigned p = 0; p < callbackData.getNbOverlappingPairs(); p++)
        {
            auto overlappair = callbackData.getOverlappingPair(p);
            OnTrigger.invoke(overlappair);
        }
    }

    rp3d::decimal HitEventListener::notifyRaycastHit(const rp3d::RaycastInfo& info)
    {
		OnHit.invoke(info);
        return rp3d::decimal(1.0);
    }
}