#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class SpriteSheetFactory : public Factory
    {
    private:
        /* data */
    public:
        virtual void CreateNew(const std::filesystem::path &path) override;

        virtual bool CanCreateNew() const { return true; }

        virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path) override;

        const char *GetFileFilters() const { return "SpriteSheet (*.spritesheet)\0*.spritesheet\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewSpriteSheet.spritesheet"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
