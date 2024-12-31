#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class SpriteFactory : public Factory
    {
    public:
        virtual void CreateNew(const std::filesystem::path &path);

        virtual bool CanCreateNew() const { return true; }

        virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

        const char *GetFileFilters() const { return "Sprite (*.sprite)\0*.sprite\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewSprite.sprite"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
