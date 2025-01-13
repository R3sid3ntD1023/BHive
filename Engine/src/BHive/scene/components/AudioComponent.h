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

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(AudioComponent)

} // namespace BHive