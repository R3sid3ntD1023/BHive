#include "AudioEditor.h"

namespace BHive
{
    AudioEditor::~AudioEditor()
    {
        if (mCurrentAudioPlaying)
            mCurrentAudioPlaying->Stop();
    }

    void AudioEditor::OnWindowRender()

    {
        if (!mAsset)
            return;

        TAssetEditor::OnWindowRender();

        if (ImGui::Button(!mAsset->IsPlaying() ? "Play" : "Stop"))
        {
            if (mAsset->IsPlaying())
            {
                mAsset->Stop();
                mCurrentAudioPlaying.reset();
            }
            else
            {
                if (mCurrentAudioPlaying)
                {
                    mCurrentAudioPlaying->Stop();
                }

                mAsset->Play();
                mCurrentAudioPlaying = mAsset;
            }
        }
    }
}
