#pragma once

#include "math/Transform.h"

namespace BHive
{
    struct ITransform
    {
        /* data */
        virtual void SetLocalTransform(const FTransform &) = 0;
 
        virtual const FTransform &GetLocalTransform() const = 0;

        virtual void SetWorldTransform(const FTransform&) = 0;

        virtual FTransform GetWorldTransform() const = 0;

        REFLECTABLEV()
    };

    REFLECT(ITransform)
    {
        BEGIN_REFLECT(ITransform)
        REFLECT_PROPERTY("Local Transform", GetLocalTransform, SetLocalTransform);
    }

} // namespace BHive
