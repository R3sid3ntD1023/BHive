#pragma once

#include "TAssetEditor.h"
#include "audio/AudioSource.h"

namespace BHive
{

    class AudioEditor : public TAssetEditor<AudioSource>
    {
    public:
        ~AudioEditor();

    protected:
        virtual void OnWindowRender();

        virtual const char *GetFileDialogFilter() { return "Ogg (*.ogg)\0*.ogg\0Wav (*.wav)\0*.wav"; };

        virtual std::filesystem::path GetAssetPath(const std::filesystem::path &path) const override { return path.parent_path() / (path.stem().string() + ".meta"); }

        virtual Ref<AudioSource> GetNewAsset() const override { return mAsset; }

    private:
        static inline Ref<AudioSource> mCurrentAudioPlaying;
    };
}