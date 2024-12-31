#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class PhysicsMaterialFactory : public Factory
    {
    public:
        virtual void CreateNew(const std::filesystem::path &path);

        virtual bool CanCreateNew() const { return true; }

        virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

        const char *GetFileFilters() const { return "PhysicsMaterial (*.physicsmaterial)\0*.physicsmaterial\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewPhysicsMaterial.physicsmaterial"; }

        REFLECTABLEV(Factory)
    };
} // namespace BHive
