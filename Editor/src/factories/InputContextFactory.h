#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class InputContextFactory : public Factory
    {
    public:
        virtual void CreateNew(const std::filesystem::path &) override;

        virtual bool CanCreateNew() const override { return true; }

        virtual bool Import(Ref<Asset> &, const std::filesystem::path &) override;

        const char *GetFileFilters() const override { return "InputContext (*.input)\0*.input\0"; }

        virtual const char* GetDefaultAssetName() const override { return "NewInputContext.input"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
