#include "EntityFactoryAudio.h"
#include "audio/AudioSource.h"
#include "scene/Entity.h"
#include "scene/components/AudioComponent.h"

namespace BHive
{
	bool EntityFactoryAudio::CanCreateEntityFrom(const FAssetMetaData &metaData)
	{
		return metaData.Type.is_derived_from<AudioSource>();
	}

	void EntityFactoryAudio::PostCreateEntity(const Ref<Asset> &asset, Ref<Entity> &entity)
	{
		entity->AddNewComponent<AudioComponent>(asset->GetName())->mAudio = Cast<AudioSource>(asset);
	}

	REFLECT(EntityFactoryAudio)
	{
		BEGIN_REFLECT(EntityFactoryAudio)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive