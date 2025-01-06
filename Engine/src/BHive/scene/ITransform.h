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

        virtual const FTransform& GetWorldTransform() const = 0;

        REFLECTABLEV()
    };

    REFLECT(ITransform)
    {
        BEGIN_REFLECT(ITransform)
        REFLECT_PROPERTY("Local Transform", GetLocalTransform, SetLocalTransform)
        REFLECT_PROPERTY("World Transform", GetWorldTransform, SetWorldTransform);
    }

} // namespace BHive
