#include "WorldEventListener.h"

namespace BHive
{
    void WorldEventListener::onContact(const rp3d::CollisionCallback::CallbackData &callbackData)
    {
        for (unsigned p = 0; p < callbackData.getNbContactPairs(); p++)
        {
            auto contactpair = callbackData.getContactPair(p);
            OnContact.invoke(contactpair);
        }
    }

    void WorldEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData &callbackData)
    {
        for (unsigned p = 0; p < callbackData.getNbOverlappingPairs(); p++)
        {
            auto overlappair = callbackData.getOverlappingPair(p);
            OnTrigger.invoke(overlappair);
        }
    }
}