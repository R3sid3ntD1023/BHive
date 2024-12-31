#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class MaterialFactory : public Factory
    {
    public:
        virtual void CreateNew(const std::filesystem::path &path);

        virtual bool CanCreateNew() const { return true; }

        virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

        const char *GetFileFilters() const { return "Material (*.material)\0*.material\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewMaterial.material"; }

        REFLECTABLEV(Factory)
    };
} // namespace BHive
