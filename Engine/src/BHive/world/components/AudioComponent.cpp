#include "AudioComponent.h"
#include "audio/AudioSource.h"
#include "GameObject.h"

namespace BHive
{
	void AudioComponent::Begin()
	{
		if (Audio && AutoPlay)
			Audio->Play();
	}

	void AudioComponent::End()
	{
		if (Audio && Audio->IsPlaying())
			Audio->Stop();
	}

	void AudioComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(AutoPlay, TAssetHandle(Audio));
	}

	void AudioComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(AutoPlay, TAssetHandle(Audio));
	}

	void AudioComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP(AutoPlay), MAKE_NVP("Audio", TAssetHandle(Audio)));
	}

	void AudioComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP(AutoPlay), MAKE_NVP("Audio", TAssetHandle(Audio)));
	}

	REFLECT(AudioComponent)
	{
		BEGIN_REFLECT(AudioComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY(AutoPlay)
		REFLECT_PROPERTY(Audio)
		COMPONENT_IMPL();
	}
} // namespace BHive