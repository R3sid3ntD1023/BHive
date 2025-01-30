#include "AudioComponent.h"

namespace BHive
{

	void AudioComponent::OnBegin()
	{
		if (mAudio && mPlayOnStart)
			mAudio->Play();
	}

	void AudioComponent::OnEnd()
	{
		if (mAudio && mAudio->IsPlaying())
			mAudio->Stop();
	}

	void AudioComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		Component::Save(ar);
		ar(mPlayOnStart, mAudio);
	}

	void AudioComponent::Load(cereal::BinaryInputArchive &ar)
	{
		Component::Load(ar);
		ar(mPlayOnStart, mAudio);
	}

	REFLECT(AudioComponent)
	{
		BEGIN_REFLECT(AudioComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Play On Start", mPlayOnStart)
			REFLECT_PROPERTY("Audio", mAudio);
	}

} // namespace BHive