#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class MaterialFactory : public Factory
    {
    public:
        virtual Ref<Asset> CreateNew() override;

        virtual bool CanCreateNew() const { return true; }

        //virtual Ref<Asset> Import(Ref<Asset> &asset, const std::filesystem::path &path) override;

        const char *GetFileFilters() const { return "Material (*.material)\0*.material\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewMaterial"; }

        REFLECTABLEV(Factory)
    };
} // namespace BHive
