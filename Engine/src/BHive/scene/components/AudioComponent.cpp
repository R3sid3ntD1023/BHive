#include "AudioComponent.h"
#include "audio/AudioSource.h"
#include "scene/Entity.h"

namespace BHive
{
    
    void AudioComponent::OnBegin()
    {
        if (mAudio && mPlayOnStart)
            mAudio->Play();
    }

    void AudioComponent::Serialize(StreamWriter& ar) const
    {
        Component::Serialize(ar);
        ar(mPlayOnStart, mAudio);
    }

    void AudioComponent::Deserialize(StreamReader& ar)
    {
        Component::Deserialize(ar);
        ar(mPlayOnStart, mAudio);
    }

    REFLECT(AudioComponent)
    {
        BEGIN_REFLECT(AudioComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS()
            REFLECT_PROPERTY("Play On Start", mPlayOnStart)
            REFLECT_PROPERTY("Audio", mAudio);
    }

} // namespace BHive
