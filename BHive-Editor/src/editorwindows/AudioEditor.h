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
		virtual void OnContentUpdate() override;

	private:
		static inline Ref<AudioSource> mCurrentAudioPlaying;
	};
} // namespace BHive