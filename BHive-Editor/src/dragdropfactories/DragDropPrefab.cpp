#include "DragDropPrefab.h"
#include "prefab/Prefab.h"
#include "asset/EditorAssetManager.h"
#include "world/GameObject.h"
#include "world/World.h"

namespace BHive
{
	bool DragDropPrefab::CanCreateEntityFrom(const rttr::type &type)
	{
		return type.is_derived_from<Prefab>();
	}

	void DragDropPrefab::PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object)
	{
		if (auto prefab = Cast<Prefab>(asset))
		{
			auto instance_world = prefab->GetInstance();
			auto world = object->GetWorld();

			for (auto &[id, gameobject] : instance_world->GetGameObjects())
			{
				auto new_object = gameobject->Duplicate();
				world->AddGameObject(new_object);
				object->AddChild(new_object.get());
			}
		}
	}

	REFLECT(DragDropPrefab)
	{
		BEGIN_REFLECT(DragDropPrefab)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive