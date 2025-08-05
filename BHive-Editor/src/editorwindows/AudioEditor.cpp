#include "AudioEditor.h"

namespace BHive
{
	AudioEditor::~AudioEditor()
	{
		if (mCurrentAudioPlaying)
			mCurrentAudioPlaying->Stop();
	}

	void AudioEditor::OnUpdateContent()

	{
		if (!mAsset)
			return;

		TAssetEditor::OnUpdateContent();

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
} // namespace BHive
