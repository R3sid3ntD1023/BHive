#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class FlipBookFactory : public Factory
    {
    public:
        virtual Ref<Asset> CreateNew() override;

        virtual bool CanCreateNew() const override { return true; }

        const char *GetFileFilters() const { return "FlipBook (*.flipbook)\0*.flipbook\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewFlipBook"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
