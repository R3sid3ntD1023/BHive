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

        virtual Ref<AudioSource> GetNewAsset() const override { return mAsset; }

    private:
        static inline Ref<AudioSource> mCurrentAudioPlaying;
    };
}