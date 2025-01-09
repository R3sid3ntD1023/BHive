#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class InputContextFactory : public Factory
    {
    public:
        virtual Ref<Asset> CreateNew() override;

        virtual bool CanCreateNew() const override { return true; }
        const char *GetFileFilters() const override { return "InputContext (*.input)\0*.input\0"; }

        virtual const char* GetDefaultAssetName() const override { return "NewInputContext"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
