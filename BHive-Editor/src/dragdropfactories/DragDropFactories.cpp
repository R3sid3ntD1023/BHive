#include "DragDropFactories.h"
#include "audio/AudioSource.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/StaticMesh.h"
#include "prefab/Prefab.h"

#include "world/GameObject.h"
#include "world/World.h"
#include "world/components/AudioComponent.h"
#include "world/components/SkeletalMeshComponent.h"
#include "world/components/StaticMeshComponent.h"

namespace BHive
{
	bool DragDropAudio::can_create(const rttr::type &type)
	{
		return type.is_derived_from<AudioSource>();
	}

	void DragDropAudio::post_create(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto audio = Cast<AudioSource>(asset))
			object->AddComponent<AudioComponent>()->Audio = audio;
	}

	bool DragDropSkeletalMesh::can_create(const rttr::type &type)
	{
		return type.is_derived_from<SkeletalMesh>();
	}

	void DragDropSkeletalMesh::post_create(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto mesh = Cast<SkeletalMesh>(asset))
			object->AddComponent<SkeletalMeshComponent>()->SkeletalMeshAsset = mesh;
	}

	bool DragDropStaticMesh::can_create(const rttr::type &type)
	{
		return type.is_derived_from<StaticMesh>();
	}

	void DragDropStaticMesh::post_create(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto mesh = Cast<StaticMesh>(asset))
			object->AddComponent<StaticMeshComponent>()->StaticMeshAsset = mesh;
	}

	bool DragDropPrefab::can_create(const rttr::type &type)
	{
		return type.is_derived_from<Prefab>();
	}

	void DragDropPrefab::post_create(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto prefab = Cast<Prefab>(asset))
		{
			auto instance_world = prefab->GetInstance();
			auto world = object->GetWorld();

			for (const auto &[id, gameobject] : instance_world->GetGameObjects())
			{
				auto duplicated = world->duplicate_gameobject(gameobject.get());
				if (duplicated)
					object->AddChild(duplicated);
			}
		}
	}

} // namespace BHive