#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class SpriteFactory : public Factory
    {
    public:
        virtual Ref<Asset> CreateNew();

        virtual bool CanCreateNew() const { return true; }

        const char *GetFileFilters() const { return "Sprite (*.sprite)\0*.sprite\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewSprite"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
