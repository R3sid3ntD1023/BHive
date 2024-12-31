#pragma once

#include "scene/ActorComponent.h"
#include "audio/AudioSource.h"

namespace BHive
{
	class AudioSource;

	struct BHIVE AudioComponent : public ActorComponent
	{
		void OnBegin() override;

		bool mPlayOnStart = false;

		TAssetHandle<AudioSource> mAudio;

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		REFLECTABLEV(ActorComponent)
	};

	REFLECT_EXTERN(AudioComponent)

}