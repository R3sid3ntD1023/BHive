#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class PhysicsMaterialFactory : public Factory
    {
    public:
        virtual Ref<Asset> CreateNew();

        virtual bool CanCreateNew() const { return true; }

        const char *GetFileFilters() const { return "PhysicsMaterial (*.physicsmaterial)\0*.physicsmaterial\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewPhysicsMaterial"; }

        REFLECTABLEV(Factory)
    };
} // namespace BHive
