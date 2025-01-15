#pragma once

#include "asset/TAssetHandler.h"
#include "audio/AudioSource.h"
#include "scene/Component.h"

namespace BHive
{
	class AudioSource;

	struct BHIVE AudioComponent : public Component
	{
		void OnBegin() override;

		bool mPlayOnStart = false;

		TAssetHandle<AudioSource> mAudio;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(AudioComponent)

} // namespace BHive