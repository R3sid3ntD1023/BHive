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

    private:
        static inline Ref<AudioSource> mCurrentAudioPlaying;
    };
}