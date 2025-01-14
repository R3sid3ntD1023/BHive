#pragma once

#include "scene/Component.h"
#include "audio/AudioSource.h"

namespace BHive
{
	class AudioSource;

	struct BHIVE AudioComponent : public Component
	{
		void OnBegin() override;

		bool mPlayOnStart = false;

		TAssetHandle<AudioSource> mAudio;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(AudioComponent)

}