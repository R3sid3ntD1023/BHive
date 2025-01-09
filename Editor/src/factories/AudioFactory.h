#pragma once

#include "asset/Factory.h"

namespace BHive
{
    class AudioFactory : public Factory
    {
    public:
        virtual bool CanCreateNew() const { return false; }

        virtual Ref<Asset> Import(const std::filesystem::path &path) override;

        const char *GetFileFilters() const { return "Ogg (*.ogg)\0*.ogg\0Wav (*.wav)\0*.wav"; }

        virtual const char* GetDefaultAssetName() const override { return "NewAudio"; }

        REFLECTABLEV(Factory)
    };

} // namespace BHive
