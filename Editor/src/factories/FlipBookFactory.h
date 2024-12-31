#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class FlipBookFactory : public Factory
    {
    public:
        virtual void CreateNew(const std::filesystem::path &path) override;

        virtual bool CanCreateNew() const override { return true; }

        virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path) override;

        const char *GetFileFilters() const { return "FlipBook (*.flipbook)\0*.flipbook\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewFlipBook.flipbook"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
