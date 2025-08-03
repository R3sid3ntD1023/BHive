#include "DragDropAudio.h"
#include "audio/AudioSource.h"
#include "world/GameObject.h"
#include "world/components/AudioComponent.h"

namespace BHive
{
	bool DragDropAudio::CanCreateEntityFrom(const rttr::type &type)
	{
		return type.is_derived_from<AudioSource>();
	}

	void DragDropAudio::PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto audio = Cast<AudioSource>(asset))
			object->AddComponent<AudioComponent>()->Audio = audio;
	}

	REFLECT(DragDropAudio)
	{
		BEGIN_REFLECT(DragDropAudio)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive