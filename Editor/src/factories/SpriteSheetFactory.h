#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class SpriteSheetFactory : public Factory
    {
    private:
        /* data */
    public:
        virtual Ref<Asset> CreateNew() override;

        virtual bool CanCreateNew() const { return true; }

        const char *GetFileFilters() const { return "SpriteSheet (*.spritesheet)\0*.spritesheet\0"; }

        virtual const char* GetDefaultAssetName() const { return "NewSpriteSheet"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
